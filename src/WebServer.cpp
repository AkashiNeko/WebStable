// WebServer.cpp

#include "WebServer.h"
#include <cassert>

namespace webstab {

WebServer::WebServer(const Config& config) : poller_(new iohub::Epoll) {
    try {
        server_.reuse_addr(true);
        server_.set_blocking(false);
        nano::AddrPort listen = config.get_listen();
        server_.bind(listen.get_addr(), listen.get_port());
        poller_->insert(server_.get_sock(), EPOLLIN | EPOLLET);
        server_.listen();
        std::cout << "Web server listening on "
            << listen.to_string() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Web server start failed: " << e.what() << std::endl;
        exit(-2);
    }
}

int WebServer::exec() {
    std::vector<iohub::fd_event_t> fd_events;
    int server_fd = server_.get_sock();
    while (true) {
        // main loop
        poller_->wait(fd_events);
        char buf[1024000];
        for (auto [fd, event] : fd_events) {
            if (fd == server_fd) {
                while (true) {
                    try {
                        nano::Socket new_sock = server_.accept();
                        new_sock.set_blocking(false);
                        int new_fd = new_sock.get_sock();
                        poller_->insert(new_fd, EPOLLIN | EPOLLET);
                        std::cout << "\raccept fd = " << new_fd;
                        fflush(stdout);
                        sock_map_[new_fd] = new_sock;
                    } catch (const std::exception& e) {
                        break;
                    }
                }
            } else {
                // link fd
                std::string msg;
                int len = 0;
                nano::Socket& socket = sock_map_[fd];
                while (true) {
                    try {
                        len = socket.receive(buf, sizeof(buf));
                        // len = recv(fd, buf, sizeof(buf) - 1, 0);
                    } catch (const std::exception& e) {
                        len = 0;
                        std::cerr << "socket exception: " << e.what() << std::endl;
                    }
                    if (len == 0) {
                        // client closed
                        poller_->erase(fd);
                        std::cout << "client closed: "
                            << socket.get_remote().to_string()
                            << ", fd = " << fd
                            << ", poller size = " << poller_->size()
                            << std::endl;
                        socket.close();
                        sock_map_.erase(fd);
                        break;
                    }
                    if (len == -1) break; // read end
                    printf("\rlength = %d        ", len);
                    fflush(stdout);
                }
            }
        }
    }
    return 0;
}

} // namespace webstab
