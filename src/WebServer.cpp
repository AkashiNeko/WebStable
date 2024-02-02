// WebServer.cpp

#include "WebServer.h"

namespace webstab {

WebServer::WebServer(const Config& config) {
    try {
        server_.reuse_addr(true);
        nano::AddrPort listen = config.get_listen();
        server_.bind(listen.get_addr(), listen.get_port());
        SocketPoller poller(PollerType::Epoll, server_.get_sock());
        server_.listen();
        std::cout << "Web server listening on " << listen.to_string() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Web server start failed: " << e.what() << std::endl;
        exit(-2);
    }
}

} // namespace webstab
