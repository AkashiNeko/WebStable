// WebServer.h

#pragma once
#ifndef WEBSTABLE_WEB_SERVER_H
#define WEBSTABLE_WEB_SERVER_H

// C++
#include <iostream>

// WebStable
#include "Config.h"
#include "SocketPoller.h"

namespace webstab {

class WebServer {
    nano::ServerSocket server_;
    iohub::PollerBase* poller_;

public:
    WebServer(const Config& config);
    // TODO
};

} // namespace webstab

#endif // WEBSTABLE_WEB_SERVER_H
