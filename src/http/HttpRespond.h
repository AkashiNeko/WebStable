// http_respond.h

#pragma once
#ifndef __HTTP_RESPOND_H__
#define __HTTP_RESPOND_H__

// C
#include <cassert>

// C++
#include <string>
#include <unordered_map>
#include <sstream>

#include "HttpRespond.h"

// nanonet
#include "nanonet.h"

namespace webstab {

struct HttpRespond {
public:

    // line
    std::string version;
    std::string status_code;
    std::string status_message;

    // headders
    std::unordered_map<std::string, std::string> headers;

    // text
    std::string body;

public:

    HttpRespond(const std::string& version = "HTTP/1.1",
                const std::string& statusCode = "200",
                const std::string& statusMessage = "OK");

    inline void set_header(const std::string& key, const std::string& value) {
        headers[key] = value;
    }

    // to string
    std::string to_string() const;

}; // class HttpRespond

} // namespace webstab

#endif // __HTTP_RESPOND_H__