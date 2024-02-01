// WebServer.h

#pragma once
#ifndef WEBSTABLE_WEB_SERVER_H
#define WEBSTABLE_WEB_SERVER_H

// C++
#include <iostream>

// WebStable
#include "Config.h"

// NanoNet
#include "nanonet.h"

// IOHub
#include "iohub.h"

namespace webstab {

class WebServer {
public:
    WebServer(const Config& config);
    // TODO
};

} // namespace webstab

#endif // WEBSTABLE_WEB_SERVER_H
