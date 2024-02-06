// File:     iohub.h
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

#pragma once
#ifndef IOHUB_H
#define IOHUB_H

// C
#include <cerrno>
#include <cstring>

// C++
#include <exception>
#include <utility>
#include <queue>
#include <string>
#include <map>
#include <unordered_map>

// Linux
#include <unistd.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/epoll.h>

namespace iohub {

// except
class IOHubExcept : public std::exception {
    std::string except_msg_;
public:
    explicit IOHubExcept(const std::string& msg) : except_msg_(msg) {}
    explicit IOHubExcept(std::string&& msg) : except_msg_(std::move(msg)) {}
    virtual ~IOHubExcept() override = default;
    virtual const char* what() const noexcept override {
        return except_msg_.c_str();
    }
};

// type pair {fd: int, event: int}
using fd_event_t = std::pair<int, int>;

// enum Event
enum Event {
    IOHUB_IN  = 0x01,
    IOHUB_PRI = 0x02,
    IOHUB_OUT = 0x04,
};

// PollerBase
class PollerBase {
public:
    // ctor & dtor
    PollerBase() = default;
    virtual ~PollerBase() = default;

    // uncopyable
    PollerBase(const PollerBase&) = delete;
    PollerBase& operator=(const PollerBase&) = delete;

    virtual void insert(int fd, int events) = 0;
    virtual void erase(int fd) = 0;
    virtual void modify(int fd, int events) = 0;
    virtual size_t size() const noexcept = 0;
    virtual void clear() noexcept = 0;

    virtual size_t wait(std::vector<fd_event_t>& fdevt_arr,
        int timeout = -1) = 0;

    virtual bool is_open() const noexcept = 0;
    virtual void close() noexcept = 0;

}; // class PollerBase


// Select
class Select : public PollerBase {
    std::vector<unsigned char> fd_hasharr_;
    std::vector<fd_event_t> cache_;
    size_t max_, size_, readsz_, writesz_, exceptsz_;
    fd_set readfds_, writefds_, exceptfds_;
    bool is_open_;

public:
    Select();
    virtual ~Select() override = default;

    virtual void insert(int fd, int events) override;
    virtual void erase(int fd) override;
    virtual void modify(int fd, int events) override;
    virtual size_t size() const noexcept override;
    virtual void clear() noexcept override;

    virtual size_t wait(std::vector<fd_event_t>& fdevt_arr,
        int timeout = -1) override;

    virtual bool is_open() const noexcept override;
    virtual void close() noexcept override;

}; // class Select

// Poll
class Poll : public PollerBase {
    std::vector<size_t> fd_map_;
    std::vector<pollfd> pollfd_arr_;
    bool is_open_;

public:
    Poll();
    virtual ~Poll() override = default;

    virtual void insert(int fd, int events) override;
    virtual void erase(int fd) override;
    virtual void modify(int fd, int events) override;
    virtual size_t size() const noexcept override;
    virtual void clear() noexcept override;

    virtual size_t wait(std::vector<fd_event_t>& fdevt_arr,
        int timeout = -1) override;

    virtual bool is_open() const noexcept override;
    virtual void close() noexcept override;

}; // class Poll


// Epoll
class Epoll : public PollerBase {

    int epoll_fd_;
    size_t size_;
    epoll_event* event_arr_;

public:
    Epoll();
    virtual ~Epoll() = default;

    virtual void insert(int fd, int events) override;
    virtual void erase(int fd) override;
    virtual void modify(int fd, int events) override;
    virtual size_t size() const noexcept override;
    virtual void clear() noexcept override;

    virtual size_t wait(std::vector<fd_event_t>& fdevt_arr,
        int timeout = -1) override;

    virtual bool is_open() const noexcept override;
    virtual void close() noexcept override;

}; // class Epoll

} // namespace iohub

#endif // IOHUB_H
