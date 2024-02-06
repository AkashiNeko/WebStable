// ThreadPool.h

#pragma once
#ifndef WEBSTABLE_THREAD_POOL_H
#define WEBSTABLE_THREAD_POOL_H

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace webstab {

class ThreadPool {

    // task types
    using func_t = void (*)(void *);
    using task_t = struct { func_t func; void* arg; };

    // task queue
    std::queue<task_t> task_queue_;

    // pthread_t array
    std::vector<pthread_t> threads_;

    // mutex
    std::mutex mutex_;

    std::unique_lock<std::mutex> lock_;

    // condition variable
    std::condition_variable cond_;

    // running flag
    bool running_;

private:

    // thread routine
    static void* thread_routine(void* thread_pool) {
        // get thread pool "this" pointer
        ThreadPool* tp = static_cast<ThreadPool*>(thread_pool);
        // if is running
        while (tp->running_) {
            
            tp->mutex_.lock(); // lock queue
            
            // wait for task
            while (tp->running_ && tp->task_queue_.empty())
                tp->cond_.wait(tp->lock_);

            if (tp->task_queue_.empty()) {
                tp->mutex_.unlock(); // unlock queue
                continue;
            }
            // get task
            task_t task = tp->task_queue_.front();
            tp->task_queue_.pop();

            tp->mutex_.unlock(); // unlock queue

            // execute task
            task.func(task.arg);

        } // while
        return nullptr;
    }

public:

    // constructor
    ThreadPool(size_t thread_num) : threads_(thread_num), lock_(mutex_) {}

    // destructor
    ~ThreadPool() {
        // shutdown
        if (running_) this->shutdown();
    }

    void run() {
        // set running flag
        running_ = true;

        // create threads
        for (pthread_t& pth : threads_)
            pthread_create(&pth, nullptr, thread_routine, (void*)this);
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
        for (const pthread_t& tid : threads_)
            pthread_join(tid, nullptr);

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
    inline void push(func_t task, void* arg) {
        mutex_.lock();

        // push task to task queue
        task_queue_.push({task, arg});
        
        // signal to thread
        cond_.notify_one();
        
        mutex_.unlock();
    }

}; // class ThreadPool

} // namespace webstab

#endif // WEBSTABLE_THREAD_POOL_H