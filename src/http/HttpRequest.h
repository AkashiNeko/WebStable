// http_request.h

#pragma once
#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <iostream>
#include <map>
#include <string>
#include <filesystem>
#include "Url.h"

namespace webstab {

struct HttpRequest {
public:

    // request line
    std::string method;
    std::string path;
    std::string version;

    // request headers
    std::map<std::string, std::string> headers;
    std::string host;

    // request body
    std::string body;

    // https
    bool use_ssl = false;

    // port
    nano::Port port;

public:

    HttpRequest();

    HttpRequest(const std::string& method,
        const Url& url, const std::string& version);

    std::string to_string() const;

    std::string lower_header(const std::string& key) const;

    std::string relative_path() const;

};  // class HttpRequest

}  // namespace webstab

#endif  // __HTTP_REQUEST_H__