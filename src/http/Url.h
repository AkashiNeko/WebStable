// File:     src/http/Url.h
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
#ifndef WEBSTABLE_HTTP_URL_H
#define WEBSTABLE_HTTP_URL_H

#include <string>
#include <unordered_map>
#include "nanonet.h"

namespace webstab {

// +-----------------------------------------------------------+
// | reference:  https://datatracker.ietf.org/doc/html/rfc3986 |
// +-----------------------------------------------------------+
// |    foo://example.com:8042/over/there?name=ferret#nose     |
// |    \_/   \______________/\_________/ \_________/ \__/     |
// |     |           |            |            |        |      |
// |  scheme     authority       path        query  fragment   |
// |     |   _____________________|__                          |
// |    / \ /                        \                         |
// |    urn:example:animal:ferret:nose                         |
// +-----------------------------------------------------------+

struct Url {
public:
    std::string raw;

    std::string scheme;
    std::string user;
    std::string password;
    std::string host;
    std::string path = "/";
    std::string query;
    std::string fragment;
    nano::Port port = 0;

private:

    void parse_user_info_(std::string&& userInfo);
    void parse_authority_(std::string&& authority);
    void parse_authority_after_(std::string&& str);
    void parse_url_();

public:

    Url() = default;
    Url(std::string&& url);
    Url(const std::string& url);
    Url(Url&& url);

    inline Url(const char* url) :Url(std::string(url)) {}

    virtual ~Url() = default;

    void parse(std::string&& url);
    void parse(const std::string& url);

    std::string authority_after() const;
    void authority_after(const std::string& authority);
    void authority_after(std::string&& authority);

    std::string to_string() const;

}; // struct Url

} // namespace webstab

#endif // WEBSTABLE_HTTP_URL_H
