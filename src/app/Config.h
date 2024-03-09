// File:     src/app/Config.h
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
#ifndef WEBSTABLE_APP_CONFIG_H
#define WEBSTABLE_APP_CONFIG_H

// C++
#include <unordered_map>
#include <filesystem>
#include <string>

// nanonet
#include "nanonet.h"

namespace webstab {

class Config {

    // [server]
    std::unordered_map<std::string, std::string> server_;

    // [static]
    std::unordered_map<std::string, std::filesystem::path> static_;

    // [error]
    std::unordered_map<std::string, std::string> error_;

    // [types]
    std::unordered_map<std::string, std::string> types_;

public:
    Config();
    Config(std::filesystem::path file);

    Config(const Config&) = default;
    Config(Config&&) = default;
    
    std::string to_string() const;

    nano::AddrPort get_listen() const;
    void set_listen(const nano::AddrPort& addr_port);
    std::string server(const std::string& name) const;
    size_t threads_num() const;
    std::string poller() const;
    std::string type(const std::string& extension) const;
    std::string server_name() const;
    std::filesystem::path static_path(std::string url_path) const;
    size_t keepalive_timeout() const;

}; // class Config

} // namespace webstab

#endif // WEBSTABLE_APP_CONFIG_H
