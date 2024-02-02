// WebServer.cpp

#include "WebServer.h"

namespace webstab {

WebServer::WebServer(const Config& config)
    : poller_(PollerType::Epoll)
{
    try {
        server_.reuse_addr(true);
        nano::AddrPort listen = config.get_listen();
        server_.bind(listen.get_addr(), listen.get_port());
        poller_.set_server(server_.get_sock());
        server_.listen();
        std::cout << "Web server listening on " << listen.to_string() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Web server start failed: " << e.what() << std::endl;
        exit(-2);
    }
}

int WebServer::exec() {
    while (true) {
        // main loop
        auto [is_serv, socket] = poller_.wait();
        if (is_serv) {
            nano::Socket new_sock = server_.accept();
            std::cout << "accepted a new link: "
                << new_sock.get_remote().to_string() << std::endl;
            new_sock.set_blocking(false);
            poller_.insert(new_sock, EPOLLIN | EPOLLET);
        } else if (socket) {
            char buf[1024];
            std::string msg;
            int len = 0;
            while (true) {
                len = socket->receive(buf, sizeof(buf) - 1);
                if (len == 0) {
                    // client closed
                    poller_.erase(*socket);
                    socket->close();
                    std::cout << "client closed: "
                        << socket->get_remote().to_string() << std::endl;
                    break;
                }
                if (len == -1) break; // read end
                buf[len] = '\0';
                msg += buf;
            }
            if (len == -1)
                std::cout << "receive: " << buf << std::endl;
        }
    }
    return 0;
}

} // namespace webstab
