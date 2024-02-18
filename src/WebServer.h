// WebServer.h

#pragma once
#ifndef WEBSTABLE_WEB_SERVER_H
#define WEBSTABLE_WEB_SERVER_H

// C++
#include <iostream>
#include <mutex>

// WebStable
#include "Config.h"
#include "EventQueue.h"

// iohub
#include "iohub.h"

namespace webstab {

class WebServer final {
    int pipe_[2];
    nano::ServerSocket server_;
    iohub::Epoll epoll;

public:
    WebServer(const Config& config);
    ~WebServer();
    WebServer(const WebServer&) = delete;
    WebServer& operator=(const WebServer&) = delete;
    WebServer(WebServer&&) = delete;
    WebServer& operator=(WebServer&&) = delete;
    int exec();
};

} // namespace webstab

#endif // WEBSTABLE_WEB_SERVER_H
