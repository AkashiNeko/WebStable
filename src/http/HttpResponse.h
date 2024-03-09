// File:     src/http/HttpResponse.h
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
#ifndef WEBSTABLE_HTTP_HTTPRESPONSE_H
#define WEBSTABLE_HTTP_HTTPRESPONSE_H

// C++
#include <string>
#include <unordered_map>

namespace webstab {

struct HttpResponse {
public:
    std::string version;
    std::string status_code;
    std::string status_message;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

public:

    HttpResponse(const std::string& version = "HTTP/1.1",
                const std::string& status_code = "200",
                const std::string& status_message = "OK");

    inline void set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }

    // to string
    std::string to_string() const;

}; // struct HttpResponse

} // namespace webstab

#endif // WEBSTABLE_HTTP_HTTPRESPONSE_H
