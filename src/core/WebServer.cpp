// WebServer.cpp

#include "WebServer.h"

namespace webstab {

namespace {

int PollerEvent = 0;

iohub::PollerBase* create_poller_(const std::string& poller_name) {
    std::cout << "create poller " << poller_name << std::endl;
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

WebServer::WebServer(const Config& config)
        : tp(config.threads_num()), poller_(create_poller_(config.poller())) {
    // listen
    nano::AddrPort listen = config.get_listen();
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
                // printf("socket %d closed\n", sock);
                nano::close_socket(sock);
                return;
            } else if (len == -1) {
                // TODO: receive done
                // cout << "receive done, length = " << read_length << endl;
                // std::cout << request.to_string() << std::endl;
                const auto& s = request.path;
                if (s.size() < 5) break;

                HttpRespond respond;
                size_t pos1 = s.find_first_of('?');
                size_t pos2 = s.find_first_of('+');
                int n1 = 0, n2 = 0;
                try {
                    n1 = std::stoi(s.substr(pos1 + 1, pos2 - pos1 - 1));
                    n2 = std::stoi(s.substr(pos2 + 1));
                } catch (const std::exception& e) {
                    respond.body = e.what();
                }
                respond.body = "<h1>" + std::to_string(n1) + " + "
                    + std::to_string(n2) + " = "
                    + std::to_string(n1 + n2) + "</h1>\n";
                respond.headers.insert({"Server", "WebStable"});
                respond.headers.insert({"Content-Type", "text/html"});
                respond.headers.insert({"Content-Length", std::to_string(respond.body.size())});

                std::string res = respond.to_string();
                // std::cout << res << std::endl;
                nano::send_msg(sock, res.c_str(), res.size());
                // printf("socket %d closed\n", sock);
                nano::close_socket(sock);
                return;
                // break;
            }
            // TODO: append message
            ha.append(buf);
        }
        poller_->insert(sock, PollerEvent);
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
        poller_->wait(fd_events);
        for (const auto& [fd, event] : fd_events) {
            if (fd == serv) {
                // new link
                while (true) {
                    auto sock = nano::accept_from(serv, nullptr, nullptr);
                    if (sock == INVALID_SOCKET) break;
                    nano::set_blocking(sock, false);
                    poller_->insert(sock, PollerEvent);
                    // printf("accepted fd = %d, poller size = %zu\n", sock, epoll.size());
                }
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
