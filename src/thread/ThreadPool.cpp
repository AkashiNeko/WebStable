// File:     src/thread/ThreadPool.cpp
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

#include "ThreadPool.h"

namespace webstab {

ThreadPool::ThreadPool(size_t thread_num) {
    for (size_t i = 0; i < thread_num; ++i)
        threads_.emplace_back(thread_routine, this);
    running_ = true;
}

ThreadPool::~ThreadPool() {
    // shutdown
    if (running_) this->shutdown();
}

void ThreadPool::set_task(task_t&& task) {
    this->task_ = std::move(task);
}

// shutdown
void ThreadPool::shutdown() {
    // is shutdown?
    if (!running_) return;
    
    // set running flag
    running_ = false;

    // broadcast to all threads
    cond_.notify_all();

    // join threads
    for (std::thread& thread : threads_)
        thread.join();

    threads_.clear();
}

// is_running
bool ThreadPool::is_running() {
    mutex_.lock();

    // get running flag
    bool ret = running_;
    
    mutex_.unlock();
    return ret;
}

// push task
void ThreadPool::push(nano::sock_t sock) {
    mutex_.lock();
    // push task to task queue
    task_queue_.push(sock);
    // signal to thread
    cond_.notify_one();
    
    mutex_.unlock();
}

void thread_routine(ThreadPool* tp) {
    // if is running
    try {
        while (tp->running_) {
            nano::sock_t sock = INVALID_SOCKET;
            {    
                std::unique_lock<std::mutex> lock(tp->mutex_);
                // wait for task
                while (tp->running_ && tp->task_queue_.empty())
                    tp->cond_.wait(lock);
                if (tp->task_queue_.empty()) {
                    tp->mutex_.unlock(); // unlock queue
                    continue;
                }
                // get task
                sock = tp->task_queue_.front();
                tp->task_queue_.pop();
            }
            // execute task
            tp->task_(sock);

        } // while
    } catch (...) {
        return;
    }
}

} // namespace webstab
