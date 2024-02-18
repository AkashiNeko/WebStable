// WebServer.cpp

#include "WebServer.h"

namespace webstab {

WebServer::WebServer(const Config& config) : tp(16) {
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
    tp.set_task([this](nano::sock_t sock) {
        size_t read_length = 0;
        int len = 0;
        char buf[10240]{};
        HttpRequest request;
        HttpAssembler ha(request);
        while (true) {
            try {
                len = nano::recv_msg(sock, buf, sizeof(buf) - 1);
            } catch (...) {
                len = 0;
            }
            if (len == 0) {
                // close
                printf("socket %d closed\n", sock);
                nano::close_socket(sock);
                return;
            } else if (len == -1) {
                // TODO: receive done
                // cout << "receive done, length = " << read_length << endl;
                std::cout << request.toString() << std::endl;
                auto s = request.getText();
                if (s.size() < 5) break;

                HttpRespond respond;
                respond.setText(std::to_string((s[2] & 0xF) + (s[4] & 0xF)));
                std::string res = respond.toString();
                nano::send_msg(sock, res.c_str(), res.size());
                break;
            }
            // TODO: append message
            ha.append(buf);
        }
        epoll.insert(sock, EPOLLIN | EPOLLET);
    });
}

WebServer::~WebServer() {
    server_.close();
}

int WebServer::exec() {
    nano::sock_t serv = server_.get();
    std::vector<iohub::fd_event_t> fd_events;
    while (true) {
        // main loop
        epoll.wait(fd_events);
        for (const auto& [fd, event] : fd_events) {
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
                epoll.erase(fd);
                tp.push(fd);
            }
        }
    }
    return 0;
}

} // namespace webstab
