// File:     src/http/HttpRequest.cpp
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

#include "HttpRequest.h"

namespace webstab {

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const std::string& method,
            const Url& url,
            const std::string& version)
        : method(method),
        path(url.authority_after()),
        version(version),
        host(url.host),
        port(url.port),
        use_ssl(url.scheme == "https") {}

std::string HttpRequest::to_string() const {
    std::string request = method + ' ' + path + ' ' + version + "\r\n";

    for (const auto& e : headers)
        request += e.first + ": " + e.second + "\r\n";

    request += "\r\n" + body;
    return request;
}

std::string HttpRequest::lower_header(const std::string& key) const {
    auto it = headers.find(key);
    if (it == headers.end()) return "";
    std::string result(it->second);
    for (char& c : result)
        if (c > 64 && c < 91) c |= 0x20;
    return result;
}

std::string HttpRequest::relative_path() const {
    if (path.size() < 2) return "";
    return path.substr(1);
}

bool HttpRequest::keep_alive() const {
    std::string&& connection = lower_header("Connection");
    if (connection == "keep-alive") return true;
    else if (connection == "close") return false;
    return version >= "HTTP/1.1";
}

} // namespace webstab
