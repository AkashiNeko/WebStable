// http_request.h

#pragma once
#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <iostream>
#include <map>
#include <string>

#include "url.h"

namespace webstab {

struct HttpRequest {
public:

    // request line
    std::string method;
    std::string path;
    std::string version;

    // request headers
    std::map<const std::string, std::string> headers;
    std::string host;

    // request body
    std::string body;

    // https
    bool useSSL = false;

    // port
    nano::Port port;

public:

    HttpRequest();

    HttpRequest(const std::string& method,
        const Url& url, const std::string& version = "HTTP/1.0");

    inline void set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }

    std::string to_string() const;

};  // class HttpRequest

}  // namespace webstab

#endif  // __HTTP_REQUEST_H__