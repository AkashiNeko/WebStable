// File:     src/EventQueue.cpp
// Author:   AkashiNeko
// Project:  iohub
// Github:   https://github.com/AkashiNeko/iohub/

/* Copyright AkashiNeko. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 *
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "EventQueue.h"

namespace iohub {

EventQueue::EventQueue() : front_(-1) {}

bool EventQueue::empty() const {
    return front_ == -1;
}

void EventQueue::push(int fd, int event) {
    if (vec_.size() <= fd)
        vec_.resize(fd + 1);
    Node& node = vec_[fd];
    node.event = event;
    if (front_ == -1) {
        front_ = fd;
        node.prev = node.next = fd;
    } else {
        Node& head = vec_[front_];
        node.next = front_;
        node.prev = head.prev;
        vec_[head.prev].next = fd;
        head.prev = fd;
    }
}

fd_event_t EventQueue::pop() {
    if (front_ == -1) return {-1, 0};
    Node& head = vec_[front_];
    fd_event_t result = {front_, head.event};
    head.event = 0;
    if (front_ == head.next) {
        front_ = -1;
    } else {
        front_ = head.next;
        vec_[front_].prev = head.prev;
        vec_[head.prev].next = front_;
    }
    return result;
}

void EventQueue::erase(int fd) {
    if (front_ == -1 || fd >= vec_.size()) return;

    // gets the node to be erased
    Node& cur = vec_[fd];

    // if the fd to be erased exists
    if (cur.event) {
        cur.event = 0; // erase
        if (cur.next == fd) {
            front_ = -1;
        } else {
            if (fd == front_)
                front_ = vec_[front_].next;
            vec_[cur.next].prev = cur.prev;
            vec_[cur.prev].next = cur.next;
        }
    }
}

void EventQueue::clear() {
    front_ = -1;
    vec_.clear();
}

}