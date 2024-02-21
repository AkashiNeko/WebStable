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
#include "respond.h"
#include "app/Config.h"
#include "http/HttpAssembler.h"

// iohub
#include "iohub.h"

namespace webstab {

class WebServer final {
    Config config_;
    int pipefd_[2];
    nano::ServerSocket server_;
    std::unique_ptr<iohub::PollerBase> poller_;
    ThreadPool tp;

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
