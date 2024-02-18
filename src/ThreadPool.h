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

using task_t = std::function<void(nano::sock_t)>;

class ThreadPool final {

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
    ThreadPool(size_t thread_num) noexcept;

    // destructor
    ~ThreadPool() noexcept;

    void set_task(task_t task) noexcept;

    // shutdown
    void shutdown();

    // is_running
    bool is_running();

    // push task
    void push(nano::sock_t sock);

private:
    friend void thread_routine(ThreadPool* tp);

}; // class ThreadPool

void thread_routine(ThreadPool* tp);

} // namespace webstab

#endif // WEBSTABLE_THREAD_POOL_H
