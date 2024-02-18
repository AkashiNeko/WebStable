// http_request.h

#pragma once
#ifndef __HTTP_REQUEST_H__
#define __HTTP_REQUEST_H__

#include <iostream>
#include <map>
#include <string>

#include "url.h"

namespace webstab {

class HttpRequest {
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

    inline size_t size() const {
        return body.size();
    }

    inline void setMethod(const std::string& method) {
        this->method = method;
        for (char& c : this->method)
            c = std::toupper(c);
    }

    inline const std::string& getMethod() const {
        return this->method;
    }

    inline void setVersion(const std::string& version) {
        this->version = version;
    }

    inline const std::string& getVersion() const {
        return this->version;
    }

    inline void setPath(const std::string& path) {
        this->path = path;
    }

    inline const std::string& getPath() const {
        return this->path;
    }

    inline void setHeader(const std::string& name, const std::string& value) {
        this->headers[name] = value;
    }

    inline void setBody(const std::string& body) {
        if (!body.empty()) {
            this->body = body;
            headers["Content-Length"] = std::to_string(body.size());
        }
    }

    inline void setPort(const nano::Port& port) {
        this->port = port;
    }

    inline bool usingSSL() const {
        return useSSL;
    }

    inline std::string getHost() const {
        return this->host;
    }

    inline nano::Port getPort() const {
        return this->port;
    }

    inline const std::map<const std::string, std::string>& getHeaders() const {
        return this->headers;
    }

    inline std::map<const std::string, std::string>& getHeaders() {
        return this->headers;
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

    std::string toString() const;

};  // class HttpRequest

}  // namespace webstab

#endif  // __HTTP_REQUEST_H__