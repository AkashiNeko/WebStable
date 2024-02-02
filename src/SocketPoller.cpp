// SocketPoller.cpp

#include "SocketPoller.h"

namespace webstab {

namespace {

iohub::PollerBase* create_poller_(PollerType type) {
    switch (type) {
    case PollerType::Epoll:
        return new iohub::Epoll;
    case PollerType::Poll:
        return new iohub::Poll;
    case PollerType::Select:
        return new iohub::Select;
    default:
        assert_throw(false, "Unknown poller type");
        return nullptr;
    }
}

} // anonymous namespace

SocketPoller::SocketPoller(PollerType type, int server_fd)
    : poller_(create_poller_(type))
    , server_fd_(server_fd)
{
}

void SocketPoller::insert(nano::Socket& sock, int event) {
    int fd = sock.get_sock();
    poller_->insert(fd, event);
    sock_map_.insert({fd, sock});
}

void SocketPoller::erase(nano::Socket& sock) {
    int fd = sock.get_sock();
    poller_->erase(fd);
    sock_map_.erase(fd);
}

std::pair<bool, nano::Socket*> SocketPoller::wait(int timeout) {
    auto [fd, _] = poller_->wait(timeout);
    if (fd == -1) return {false, nullptr};
    if (fd == server_fd_) return {true, nullptr};
    return {false, &sock_map_[fd]};
}

} // namespace webstab
