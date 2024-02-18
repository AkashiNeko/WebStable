// thread_pool.hpp

#pragma once
#ifndef WEBSTABLE_THREAD_POOL_H
#define WEBSTABLE_THREAD_POOL_H

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "nanonet.h"
#include "iohub.h"

namespace webstab {

class ThreadPool;
void thread_routine(ThreadPool* tp);
using task_t = bool(*)(nano::sock_t);

class ThreadPool {


    iohub::Epoll& epoll_;

    // task types
    task_t task_;

    // task queue
    std::queue<nano::sock_t> task_queue_;

    // pthread_t array
    std::vector<std::thread> threads_;

    // mutex
    std::mutex mutex_;

    // condition variable
    std::condition_variable cond_;

    // running flag
    bool running_;

private:

    // thread routine


public:

    // constructor
    ThreadPool(size_t thread_num, task_t task, iohub::Epoll& epoll)
            : task_(task), epoll_(epoll) {
        for (size_t i = 0; i < thread_num; ++i)
            threads_.emplace_back(thread_routine, this);
        running_ = true;
    }

    // destructor
    ~ThreadPool() {
        // shutdown
        if (running_) this->shutdown();
    }

    // shutdown
    void shutdown() {
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
    inline bool is_running() {
        mutex_.lock();

        // get running flag
        bool ret = running_;
        
        mutex_.unlock();
        return ret;
    }

    // push task
    inline void push(nano::sock_t sock) {
        mutex_.lock();
        // push task to task queue
        task_queue_.push(sock);

        // signal to thread
        cond_.notify_one();
        
        mutex_.unlock();
    }

private:
    friend void thread_routine(ThreadPool* tp);

}; // class ThreadPool

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
            if (tp->task_(sock)) {
                tp->epoll_.erase(sock);
                nano::close_socket(sock);
                printf("Socket closed: %d, poller size = %zu\n",
                    sock, tp->epoll_.size());
            }

        } // while
    } catch (...) {
        std::cerr << "failed" << std::endl;
        return;
    }
}

} // namespace webstab

#endif // WEBSTABLE_THREAD_POOL_H
