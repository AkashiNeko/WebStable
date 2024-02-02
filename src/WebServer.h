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
    SocketPoller poller_;

public:
    WebServer(const Config& config);
    int exec();
    // TODO
};

} // namespace webstab

#endif // WEBSTABLE_WEB_SERVER_H
