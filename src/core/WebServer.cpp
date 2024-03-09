// File:     src/core/WebServer.cpp
// Author:   AkashiNeko
// Project:  WebStable
// Github:   https://github.com/AkashiNeko/WebStable/

/* Copyright (c) 2024 AkashiNeko
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "WebServer.h"

// C++
#include <iostream>

// os
#include <signal.h>

// WebStable
#include "core/Responser.h"
#include "http/RequestReceiver.h"

namespace webstab {

iohub::PollerBase* WebServer::select_poller_(const std::string& poller_name) {
    if (poller_name == "select") {
        poller_event_ = iohub::IOHUB_IN;
        return new iohub::Select;
    } else if (poller_name == "poll") {
        poller_event_ = POLLIN;
        return new iohub::Poll;
    } else if (poller_name == "epoll") {
        poller_event_ = EPOLLIN | EPOLLET;
        return new iohub::Epoll;
    } else {
        std::cerr << "unsupported poller: " << poller_name << std::endl;
        exit(1);
    }
}

bool WebServer::insert_sock_(nano::sock_t sock) {
    return -1 != ::write(insert_pipe_[1], &sock, sizeof(sock));
}

WebServer::WebServer(const Config& config)
        : config_(config), insert_pipe_{-1, -1},
        thread_pool_(config_.threads_num()),
        poller_(select_poller_(config_.poller())),
        timer_(config_.keepalive_timeout()) {
    // make pipe
    if (-1 == ::pipe(insert_pipe_))
        throw std::strerror(errno);
    ::signal(SIGPIPE, SIG_IGN);
    poller_->insert(insert_pipe_[0], poller_event_);
    // listen
    nano::AddrPort listen = config_.get_listen();
    server_socket_.reuse_addr(true);
    server_socket_.set_blocking(false);
    server_socket_.set_option(SOL_SOCKET, SO_LINGER, linger{1, 1});
    try {
        server_socket_.bind(listen.addr(), listen.port());
        poller_->insert(server_socket_.get(), poller_event_);
        server_socket_.listen();
    } catch (const std::exception& e) {
        std::cerr << "Web server start failed: " << e.what() << std::endl;
        exit(-2);
    }
    std::cout << "Web server listening on " << listen.to_string() << std::endl;

    // keep-alive timer
    timer_.start();

    thread_pool_.set_task([this](nano::sock_t sock) {
        char buf[10240]{};
        HttpRequest request;
        RequestReceiver ha(request);
        while (true) {
            int recv_length = 0;
            try {
                recv_length = nano::recv_msg(sock, buf, sizeof(buf) - 1);
            } catch (const nano::NanoExcept &e) { (void)0; }

            if (recv_length == 0) {
                this->timer_.cancel(sock);
                nano::close_socket(sock);
                return;
            } else if (recv_length == -1) {
                // TODO: receive done
                if (Responser(config_, request, sock).reply()
                        && insert_sock_(sock)) {
                    this->timer_.timing(sock);
                } else {
                    this->timer_.cancel(sock);
                    nano::close_socket(sock);
                }
                break;
            } else {
                // TODO: append message
                ha.append(buf);
            }
        }
    });
}

WebServer::~WebServer() {
    server_socket_.close();
    poller_->close();
    thread_pool_.shutdown();
    timer_.stop();
    std::cout << "webserver closed" << std::endl;
}

int WebServer::exec() {
    nano::sock_t serv = server_socket_.get();
    std::vector<iohub::fd_event_t> fd_events;
    while (true) {
        // main loop
        poller_->wait(fd_events);
        for (const auto& [fd, _] : fd_events) {
            if (fd == serv) {
                // new link
                while (true) {
                    // accept new link
                    auto sock = nano::accept_from(serv, nullptr, nullptr);
                    struct linger tmp = {1, 1};
                    setsockopt(sock, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));
                    if (sock == INVALID_SOCKET) break;
                    nano::set_blocking(sock, false);
                    poller_->insert(sock, poller_event_);
                }
            } else if (fd == insert_pipe_[0]) {
                // insert to poller
                nano::sock_t add_sock = INVALID_SOCKET;
                ssize_t read_result = ::read(fd, &add_sock, sizeof(add_sock));
                if (read_result > 0) {
                    try {
                        poller_->insert(add_sock, poller_event_);
                    } catch (const iohub::IOHubExcept& e) {
                        timer_.cancel(add_sock);
                        // printf(e.what());
                    }
                }
            } else {
                // link fd
                timer_.cancel(fd);
                poller_->erase(fd);
                thread_pool_.push(fd);
            }
        }
    }
    return 0;
}

} // namespace webstab
