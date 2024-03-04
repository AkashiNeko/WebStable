#include "TimerWheel.h"

namespace webstab {

void TimerWheel::insert_nolock_(nano::sock_t socket) {
    std::size_t in = (index_ ? index_ : size_) - 1;
    map_.emplace(socket, std::pair{in, wheel_.size()});
    wheel_[in].push_back(socket);
}

void TimerWheel::remove_nolock_(nano::sock_t socket) {
    const auto it = map_.find(socket);
    if (it == map_.end()) return;
    const auto [wheel_index, list_index] = it->second;
    map_.erase(it);
    std::vector<nano::sock_t>& socks = wheel_[wheel_index];
    socks[list_index] = socks.back();
    socks.pop_back();
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

void TimerWheel::timing(nano::sock_t socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    insert_nolock_(socket);
}

void TimerWheel::cancel(nano::sock_t socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    remove_nolock_(socket);
}

void TimerWheel::update(nano::sock_t socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    remove_nolock_(socket);
    insert_nolock_(socket);
}

} // namespace webstab
