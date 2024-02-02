// SocketPoller.h

#pragma once
#ifndef WEBSTABLE_SOCKET_POLLER_H
#define WEBSTABLE_SOCKET_POLLER_H

// C++
#include <unordered_map>
#include <memory>

// NanoNet
#include "nanonet.h"

// IOHub
#include "iohub.h"

// WebStable
#include "except.h"

namespace webstab {

enum class PollerType { Epoll, Poll, Select };

class SocketPoller {
    std::unordered_map<int, nano::Socket> sock_map_;
    std::unique_ptr<iohub::PollerBase> poller_;
    int server_fd_;
public:

    SocketPoller(PollerType type, int server_fd);
    void insert(nano::Socket& sock, int event);
    void erase(nano::Socket& sock);
    std::pair<bool, nano::Socket*> wait(int timeout = -1);

}; // class SocketPoller

} // namespace webstab

#endif // WEBSTABLE_SOCKET_POLLER_H
