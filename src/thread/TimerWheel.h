// File:     src/thread/TimerWheel.h
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

#pragma once
#ifndef WEBSTABLE_THREAD_TIMEWHEEL_H
#define WEBSTABLE_THREAD_TIMEWHEEL_H

// C++
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>

// nanonet.h
#include "nanonet.h"

namespace webstab {

class TimerWheel {

    std::size_t index_;
    const std::size_t size_;
    std::vector<std::vector<nano::sock_t>> wheel_;
    std::thread thread_;
    std::mutex mutex_;
    std::unordered_map<nano::sock_t, std::pair<std::size_t, std::size_t>> map_;
    bool running_ = false;

private:
    bool insert_(nano::sock_t socket);
    bool remove_(nano::sock_t socket);

public:
    explicit TimerWheel(std::size_t wheel_size);
    ~TimerWheel() = default;

    void start();
    void stop();
    bool timing(nano::sock_t socket);
    bool cancel(nano::sock_t socket);
    bool update(nano::sock_t socket);

}; // class TimerWheel

} // namespace webstab

#endif // WEBSTABLE_THREAD_TIMEWHEEL_H
