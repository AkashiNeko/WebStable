// File:     src/thread/ThreadPool.h
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
#ifndef WEBSTABLE_THREAD_THREADPOOL_H
#define WEBSTABLE_THREAD_THREADPOOL_H

// C++
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

// nanonet
#include "nanonet.h"

// iohub
#include "iohub.h"

namespace webstab {

class ThreadPool final {
public:
    // type
    using task_t = std::function<void(nano::sock_t)>;

private:
    task_t task_;
    std::queue<nano::sock_t> task_queue_;
    std::vector<std::thread> threads_;
    std::mutex mutex_;
    std::condition_variable cond_;
    bool running_;

    friend void thread_routine(ThreadPool* tp);

public:

    explicit ThreadPool(size_t thread_num = 8);
    ~ThreadPool();

    void set_task(task_t&& task);
    void shutdown();
    bool is_running();
    void push(nano::sock_t sock);

}; // class ThreadPool

void thread_routine(ThreadPool* tp);

} // namespace webstab

#endif // WEBSTABLE_THREAD_THREADPOOL_H
