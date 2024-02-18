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

class HttpRespond {

    // line
    std::string version;
    std::string statusCode;
    std::string statusMessage;

    // headders
    std::unordered_map<std::string, std::string> headers;

    // text
    std::string body;

public:

    HttpRespond(const std::string& version = "HTTP/1.0",
                const std::string& statusCode = "200",
                const std::string& statusMessage = "OK");

    inline size_t size() const {
        return body.size();
    }

    // version
    inline const std::string& getVersion() const {
        return version;
    }
    inline void setVersion(const std::string& newVersion) {
        this->version = newVersion;
    }

    // status code
    inline const std::string& getStatusCode() const {
        return statusCode;
    }
    inline void setStatusCode(const std::string& newStatusCode) {
        this->statusCode = newStatusCode;
    }
    inline void setStatusCode(int newStatusCode) {
        this->statusCode = std::to_string(newStatusCode);
    }

    // status message
    inline const std::string& getStatusMessage() const {
        return statusMessage;
    }
    inline void setStatusMessage(const std::string& newStatusMessage) {
        this->statusMessage = newStatusMessage;
    }

    // body
    inline const std::string& getText() const {
        return body;
    }
    inline std::string& getText() {
        return body;
    }
    inline void setText(const std::string& newText) {
        this->body = newText;
    }
    inline void setText(std::string&& newText) {
        this->body = std::move(newText);
    }
    inline void appendText(const std::string& newText) {
        this->body += newText;
    }
    inline void appendText(std::string&& newText) {
        this->body += newText;
    }

    // headers
    inline const std::string& getHeader(const std::string& headerName) {
        auto it = headers.find(headerName);
        if (it == headers.end()) throw nullptr;
        return it->second;
    }
    inline void setHeader(const std::string& key, const std::string& value) {
        headers[key] = value;
    }
    inline bool findHeader(const std::string& headerName) const {
        return this->headers.find(headerName) != this->headers.end();
    }

    inline std::unordered_map<std::string, std::string>& getHeaders() {
        return headers;
    }

    // to string
    std::string toString() const;

}; // class HttpRespond

} // namespace webstab

#endif // __HTTP_RESPOND_H__