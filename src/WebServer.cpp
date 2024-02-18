// WebServer.cpp

#include "WebServer.h"
#include "ThreadPool.hpp"

using std::cout;
using std::endl;

namespace webstab {

bool task(nano::sock_t sock) {
    size_t read_length = 0;
    int len = 0;
    char buf[10240]{};
    while (true) {
        try {
            len = nano::recv_msg(sock, buf, sizeof(buf) - 1);
        } catch (...) {
            len = 0;
        }
        if (len == 0) {
            // close
            return true;
        }
        if (len == -1) {
            // TODO: receive done
            cout << "receive done, length = " << read_length << endl;
            break;
        }
        // TODO: append message
        read_length += len;
    }
    return false;
}

WebServer::WebServer(const Config& config) {
    // listen
    nano::AddrPort listen = config.get_listen();
    server_.reuse_addr(true);
    server_.set_blocking(false);
    try {
        server_.bind(listen.addr(), listen.port());
        epoll.insert(server_.get(), EPOLLIN | EPOLLET);
        server_.listen();
    } catch (const std::exception& e) {
        std::cerr << "Web server start failed: " << e.what() << std::endl;
        exit(-2);
    }
    std::cout << "Web server listening on "
        << listen.to_string() << std::endl;
}

WebServer::~WebServer() {
    server_.close();
}

int WebServer::exec() {
    std::vector<iohub::fd_event_t> fd_events;
    nano::sock_t serv = server_.get();
    ThreadPool tp(16, task, epoll);
    while (true) {
        // main loop
        epoll.wait(fd_events);
        for (auto [fd, event] : fd_events) {
            if (fd == serv) {
                // new link
                while (true) {
                    auto sock = nano::accept_from(serv, nullptr, nullptr);
                    if (sock == INVALID_SOCKET) break;
                    nano::set_blocking(sock, false);
                    epoll.insert(sock, EPOLLIN | EPOLLET);
                    printf("accepted fd = %d, poller size = %zu\n", sock, epoll.size());
                }
            } else {
                // link fd
                tp.push(fd);
            }
        }
    }
    return 0;
}

} // namespace webstab
