#include "TimerWheel.h"

namespace webstab {

bool TimerWheel::insert_(nano::sock_t socket) {
    if (map_.find(socket) != map_.end()) 
        return false;
    std::size_t in = (index_ ? index_ : size_) - 1;
    map_.emplace(socket, std::pair{in, wheel_[in].size()});
    wheel_[in].push_back(socket);
    return true;
}

bool TimerWheel::remove_(nano::sock_t socket) {
    const auto it = this->map_.find(socket);
    if (it == this->map_.end()) return false;
    const auto [wheel_index, list_index] = it->second;

    map_.erase(it);

    std::vector<nano::sock_t>& socks = wheel_[wheel_index];
    if (socks.back() == socket) {
        socks.pop_back();
    } else {
        socks[list_index] = socks.back();
        socks.pop_back();
        map_[socks[list_index]].second = list_index;
    }
    return true;
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

bool TimerWheel::timing(nano::sock_t socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    return insert_(socket);
}

bool TimerWheel::cancel(nano::sock_t socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    return remove_(socket);
}

bool TimerWheel::update(nano::sock_t socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    remove_(socket);
    return insert_(socket);
}

} // namespace webstab
