// WebServer.cpp

#include "WebServer.h"

namespace webstab {

namespace {

int PollerEvent = 0;

iohub::PollerBase* create_poller_(const std::string& poller_name) {
    if (poller_name == "select") {
        PollerEvent = iohub::IOHUB_IN;
        return new iohub::Select;
    } else if (poller_name == "poll") {
        PollerEvent = POLLIN;
        return new iohub::Poll;
    } else if (poller_name == "epoll") {
        PollerEvent = EPOLLIN | EPOLLET;
        return new iohub::Epoll;
    } else {
        std::cerr << "unsupported poller: " << poller_name << std::endl;
        exit(1);
    }
}

} // anonymous namespace

WebServer::WebServer(const Config& config) : config_(config), pipefd_{-1, -1},
        tp(config_.threads_num()), poller_(create_poller_(config_.poller())) {
    if (-1 == ::pipe(pipefd_)) {
        std::cerr << "create pipe failed: " << std::strerror(errno) << std::endl;
        exit(1);
    }
    poller_->insert(pipefd_[0], PollerEvent);
    // listen
    nano::AddrPort listen = config_.get_listen();
    server_.reuse_addr(true);
    server_.set_blocking(false);
    try {
        server_.bind(listen.addr(), listen.port());
        poller_->insert(server_.get(), PollerEvent);
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
                // std::cout << request.to_string() << std::endl;
                if (Responser(config_, request, sock).reply()) {
                    printf("keep-alive, fd = %d\n", sock);
                    write(pipefd_[1], &sock, sizeof(sock));
                } else {
                    nano::close_socket(sock);
                    printf("connection closed, fd = %d\n", sock);
                }
                break;
            }
            // TODO: append message
            ha.append(buf);
        }
    });
}

WebServer::~WebServer() {
    server_.close();
    poller_->close();
    tp.shutdown();
    std::cout << "webserver closed" << std::endl;
}

int WebServer::exec() {
    nano::sock_t serv = server_.get();
    std::vector<iohub::fd_event_t> fd_events;
    while (true) {
        // main loop
        poller_->wait(fd_events);
        for (const auto& [fd, _] : fd_events) {
            if (fd == serv) {
                // new link
                while (true) {
                    auto sock = nano::accept_from(serv, nullptr, nullptr);
                    if (sock == INVALID_SOCKET) break;
                    nano::set_blocking(sock, false);
                    poller_->insert(sock, PollerEvent);
                    printf("\033[32maccepted fd = %d\033[0m\n", sock);
                }
            } else if (fd == pipefd_[0]) {
                // event
                nano::sock_t add_sock = INVALID_SOCKET;
                ::read(fd, &add_sock, sizeof(add_sock));
                poller_->insert(add_sock, PollerEvent);
            } else {
                // link fd
                poller_->erase(fd);
                tp.push(fd);
            }
        }
    }
    return 0;
}

} // namespace webstab
