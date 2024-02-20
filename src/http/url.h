// url.h

#pragma once
#ifndef __URL_H__
#define __URL_H__

#include <string>
#include <unordered_map>
#include "nanonet.h"
#include "../except.h"

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

class Url {

    // url string
    std::string raw_;

    // scheme
    std::string scheme_;
    
    // userinfo
    std::string user_;
    std::string password_;

    // host
    std::string host_;

    // path
    std::string path_ = "/";

    // query
    std::string query_;

    // fragment
    std::string fragment_;

    // port
    nano::Port port_ = 0;

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

    inline std::string scheme() const {
        return this->scheme_;
    }

    inline std::string user() const {
        return this->user_;
    }

    inline std::string password() const {
        return this->password_;
    }

    inline std::string host() const {
        return this->host_;
    }

    inline nano::Port port() const {
        return this->port_;
    }

    inline std::string path() const {
        return this->path_;
    }

    inline std::string query() const {
        return this->query_;
    }

    inline std::string fragment() const {
        return this->fragment_;
    }

    inline void scheme(const std::string& scheme) {
        this->scheme_ = scheme;
    }
    inline void scheme(std::string&& scheme) {
        this->scheme_ = std::move(scheme);
    }

    inline void user(const std::string& user) {
        this->user_ = user;
    }
    inline void user(std::string&& user) {
        this->user_ = std::move(user);
    }

    inline void password(const std::string& password) {
        this->password_ = password;
    }
    inline void password(std::string&& password) {
        this->password_ = std::move(password);
    }

    inline void host(const std::string& host) {
        this->host_ = host;
    }
    inline void host(std::string&& host) {
        this->host_ = std::move(host);
    }

    inline void port(nano::Port port) {
        this->port_ = port;
    }

    inline void path(const std::string& path) {
        this->path_ = path;
    }
    inline void path(std::string&& path) {
        this->path_ = std::move(path);
    }

    inline void query(const std::string& query) {
        this->query_ = query;
    }
    inline void query(std::string&& query) {
        this->query_ = std::move(query);
    }

    inline void fragment(const std::string& fragment) {
        this->fragment_ = fragment;
    }
    inline void fragment(std::string&& fragment) {
        this->fragment_ = std::move(fragment);
    }

    std::string to_string() const;

}; // class Url

} // namespace webstab

#endif // __URL_H__