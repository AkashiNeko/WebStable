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

SocketPoller::SocketPoller(PollerType type)
    : poller_(create_poller_(type))
    , server_fd_(-1)
{
}

void SocketPoller::set_server(int fd) {
    if (server_fd_ == -1) {
        server_fd_ = fd;
        poller_->insert(fd, iohub::IOHUB_IN);
    }
}

void SocketPoller::insert(const nano::Socket& sock, int event) {
    int fd = sock.get_sock();
    poller_->insert(fd, event);
    sock_map_.insert({fd, sock});
}

void SocketPoller::erase(const nano::Socket& sock) {
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

bool SocketPoller::is_open() const {
    return poller_->is_open();
}

void SocketPoller::close() {
    poller_->close();
}

} // namespace webstab
