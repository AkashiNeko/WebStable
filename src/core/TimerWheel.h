#pragma once
#ifndef WEBSTABLE_TIME_WHEEL_H
#define WEBSTABLE_TIME_WHEEL_H

#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
#include <nanonet.h>

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
    void insert_nolock_(nano::sock_t socket);
    void remove_nolock_(nano::sock_t socket);

public:

    explicit TimerWheel(std::size_t wheel_size);
    ~TimerWheel() = default;

    void start();
    void stop();
    void timing(nano::sock_t socket);
    void cancel(nano::sock_t socket);
    void update(nano::sock_t socket);
};

} // namespace webstab

#endif // WEBSTABLE_TIME_WHEEL_H
