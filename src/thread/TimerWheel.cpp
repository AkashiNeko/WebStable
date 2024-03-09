// File:     src/thread/TimerWheel.cpp
// Author:   AkashiNeko
// Project:  WebStable
// Github:   https://github.com/AkashiNeko/WebStable/

/* Copyright (c) 2024 AkashiNeko
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "TimerWheel.h"

// C++
#include <chrono>

namespace webstab {

bool TimerWheel::insert_(nano::sock_t socket) {
    if (map_.find(socket) != map_.end()) 
        return false;
    std::size_t in = (index_ ? index_ : size_) - 1;
    map_.emplace(socket, std::pair{in, wheel_[in].size()});
    wheel_[in].push_back(socket);
    return true;
}

bool TimerWheel::remove_(nano::sock_t socket) {
    const auto it = this->map_.find(socket);
    if (it == this->map_.end()) return false;
    const auto [wheel_index, list_index] = it->second;

    map_.erase(it);

    std::vector<nano::sock_t>& socks = wheel_[wheel_index];
    if (socks.back() == socket) {
        socks.pop_back();
    } else {
        socks[list_index] = socks.back();
        socks.pop_back();
        map_[socks[list_index]].second = list_index;
    }
    return true;
}

TimerWheel::TimerWheel(std::size_t wheel_size)
    : index_(0),
    size_(wheel_size),
    wheel_(size_) {}

void TimerWheel::start() {
    if (running_) return;
    running_ = true;
    thread_ = std::thread([this]() {
        while (this->running_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::lock_guard<std::mutex> lock(mutex_);
            auto& list = wheel_[index_];
            if (++index_ == size_) index_ = 0;
            for (const auto& socket : list) {
                map_.erase(socket);
                nano::close_socket(socket);
            }
            list.clear();
        }
    });
    thread_.detach();
}

void TimerWheel::stop() {
    if (running_) running_ = false;
}

bool TimerWheel::timing(nano::sock_t socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    return insert_(socket);
}

bool TimerWheel::cancel(nano::sock_t socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    return remove_(socket);
}

bool TimerWheel::update(nano::sock_t socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    remove_(socket);
    return insert_(socket);
}

} // namespace webstab
