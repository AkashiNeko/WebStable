// WebServer.h

#pragma once
#ifndef WEBSTABLE_WEB_SERVER_H
#define WEBSTABLE_WEB_SERVER_H

// C++
#include <iostream>

// WebStable
#include "Config.h"

// iohub
#include "iohub.h"

namespace webstab {

class WebServer {
    nano::ServerSocket server_;
    std::unordered_map<int, nano::Socket> sock_map_;
    std::unique_ptr<iohub::PollerBase> poller_;

public:
    WebServer(const Config& config);
    int exec();
};

} // namespace webstab

#endif // WEBSTABLE_WEB_SERVER_H
