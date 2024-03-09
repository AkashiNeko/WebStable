// File:     src/core/WebServer.h
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

#pragma once
#ifndef WEBSTABLE_CORE_WEBSERVER_H
#define WEBSTABLE_CORE_WEBSERVER_H

// C++
#include <memory>

// nanonet
#include "nanonet.h"

// iohub
#include "iohub.h"

// WebStable
#include "app/Config.h"
#include "thread/ThreadPool.h"
#include "thread/TimerWheel.h"

namespace webstab {

class WebServer final {
    Config config_;
    int insert_pipe_[2];
    int poller_event_;
    ThreadPool thread_pool_;
    std::unique_ptr<iohub::PollerBase> poller_;
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

}; // class WebServer

} // namespace webstab

#endif // WEBSTABLE_CORE_WEBSERVER_H
