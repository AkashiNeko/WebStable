// WebServer.h

#pragma once
#ifndef WEBSTABLE_WEB_SERVER_H
#define WEBSTABLE_WEB_SERVER_H

// C++
#include <iostream>
#include <mutex>
#include <memory>

// WebStable
#include "ThreadPool.h"
#include "Responser.h"
#include "app/Config.h"
#include "http/HttpAssembler.h"
#include "TimerWheel.h"
// #include "Timer.hpp"

// iohub
#include "iohub.h"

namespace webstab {

class WebServer final {
    Config config_;
    int insert_pipe_[2];
    ThreadPool thread_pool_;
    std::unique_ptr<iohub::PollerBase> poller_;
    int poller_event_;
    nano::ServerSocket server_socket_;
    TimerWheel timer_;

private:
    iohub::PollerBase* select_poller_(const std::string& poller_name);
    bool insert_sock_(nano::sock_t sock);

public:
    WebServer(const Config& config);
    ~WebServer();

    // non-copyable
    WebServer(const WebServer&) = delete;
    WebServer& operator=(const WebServer&) = delete;

    int exec();
};

} // namespace webstab

#endif // WEBSTABLE_WEB_SERVER_H
