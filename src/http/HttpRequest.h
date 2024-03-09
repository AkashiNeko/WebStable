// File:     src/http/HttpRequest.h
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
#ifndef WEBSTABLE_HTTP_HTTPREQUEST_H
#define WEBSTABLE_HTTP_HTTPREQUEST_H

// C++
#include <unordered_map>
#include <string>

// nanonet
#include "nanonet.h"

// WebStable
#include "http/Url.h"

namespace webstab {

struct HttpRequest {
public:
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string host;
    nano::Port port;
    std::string body;
    bool use_ssl = false;

public:

    HttpRequest();
    HttpRequest(const std::string& method,
        const Url& url, const std::string& version);

    std::string to_string() const;
    std::string lower_header(const std::string& key) const;
    std::string relative_path() const;
    bool keep_alive() const;

}; // struct HttpRequest

} // namespace webstab

#endif // WEBSTABLE_HTTP_HTTPREQUEST_H
