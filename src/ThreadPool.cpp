#include "ThreadPool.h"

namespace webstab {

ThreadPool::ThreadPool(size_t thread_num) noexcept {
    for (size_t i = 0; i < thread_num; ++i)
        threads_.emplace_back(thread_routine, this);
    running_ = true;
}

ThreadPool::~ThreadPool() noexcept {
    // shutdown
    if (running_) this->shutdown();
}

void ThreadPool::set_task(task_t task) noexcept {
    task_ = task;
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

} // namespace webstable