// File:     src/http/Url.cpp
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

#include "Url.h"

namespace webstab {

namespace {

inline char to_hex_(const char c) {
    int mask = c & 0xF;
    return (mask > 9 ? 'A' - 10 : '0') + mask;
}

inline bool in_ignore_(const char c, const char* ignore) {
    if (c == '\0') return false;
    for (const char* p = ignore; *p; ++p)
        if (c == *p) return true;
    return false;
}

// example:
// http://user:password@example.com:8080/abc/def?a=1&b=2#section
// \__/   \__/ \______/ \_________/ \__/\______/ \_____/ \_____/
//  |       \     /          |        |    |        |       |
// scheme   userinfo       host    port  path    query  fragment
//              \____________|______/
//                        |
//                    authority

inline nano::Port get_default_port_(const std::string& scheme) {
    const static std::unordered_map<std::string, nano::Port> defaultPortMap = {
        { "ftp", 21 }, { "ssh", 22 }, { "sftp", 22 }, { "telnet", 23 },
        { "smtp", 25 }, { "dns", 53 }, { "gopher", 70 }, { "http", 80 },
        { "pop3", 110 }, { "nntp", 119 }, { "ntp", 123 }, { "imap", 143 },
        { "snmp", 161 }, { "snmp-trap", 162 }, { "ldap", 389 }, { "https", 443 },
        { "smb", 445 }, { "smtps", 465 }, { "afp", 548 }, { "ldapssl", 636 },
        { "ldaps", 636 }, { "rsync", 873 }, { "imaps", 993 }, { "pop3s", 995 },
        { "mssql", 1433 }, { "oracle", 1521 }, { "mqtt", 1883 },
        { "rtmp", 1935 }, { "nfs", 2049 }, { "zookeeper", 2181 }, { "etcd", 2379 },
        { "mysql", 3306 }, { "rdp", 3389 }, { "svn", 3690 }, { "sybase", 5000 },
        { "xmpp", 5222 }, { "xmpps", 5223 }, { "postgresql", 5432 },
        { "rabbitmq", 5672 }, { "vnc", 5900 }, { "couchdb", 5984 }, { "redis", 6379 },
        { "irc", 6667 }, { "redis-cluster", 7000 }, { "neo4j", 7687 },
        { "consul", 8500 }, { "hadoop", 9000 }, { "cassandra", 9042 },
        { "kafka", 9092 }, { "elasticsearch", 9200 }, { "git", 9418 },
        { "hive", 10000 }, { "memcached", 11211 }, { "minecraft", 25565 },
        { "redis-sentinel", 26379 }, { "mongodb", 27017 }, { "db2", 50000 },
    };
    auto it = defaultPortMap.find(scheme);
    if (it == defaultPortMap.end())
        return 0;
    else
        return it->second;
}

static void encode_string_(std::string& result, std::string&& str, const char* ignore = "") {
    for (char c : str) {
        if (c == ' ') {
            result += '+';
        } else if (in_ignore_(c, ignore) || isalnum(c) || c == '%'
            || c == '.' || c == '_' || c == '*' || c == '-') {
            result += c;
        } else {
            result += '%';
            result += to_hex_(c >> 4);
            result += to_hex_(c);
        }
    }
}

} // anonymous namespace

// parse

void Url::parse_user_info_(std::string&& user_info) {
    // http://user:password@example.com:8080/abc/def?a=1&b=2#section
    //        \___________/
    //              |
    //           userinfo

    size_t pos = user_info.find_first_of(':');
    if (pos == std::string::npos) {
        encode_string_(this->user, std::move(user_info));
    } else {
        encode_string_(this->user, user_info.substr(0, pos));
        encode_string_(this->password, user_info.substr(pos + 1));
    }
}

void Url::parse_authority_(std::string&& authority) {
    // http://user:password@example.com:8080/abc/def?a=1&b=2#section
    //        \____________________________/
    //                      |
    //                  authority

    size_t posAt = authority.find_first_of('@');
    size_t pos = 0;
    if (posAt != std::string::npos) {
        pos = posAt + 1;
        parse_user_info_(authority.substr(0, posAt));
    }
    // parse host port
    size_t posPort = authority.find_first_of(':', pos);
    if (posPort != std::string::npos) {
        encode_string_(this->host, authority.substr(pos, posPort - pos), "/");
        pos = posPort + 1;
        std::string strPort = authority.substr(posPort + 1);
        if (strPort.empty()) {
            this->port = get_default_port_(this->scheme);
        } else {
            try {
                this->port = strPort;
            } catch (...) {
                throw "port is invalid";
            }
        }
    } else {
        this->port = get_default_port_(this->scheme);
        this->host = authority.substr(pos);
    }
}

void Url::parse_authority_after_(std::string&& str) {
    // /abc/def?a=1&b=2#section
    // \______________________/
    //            |
    //     authority after

    if (str.empty()) return;

    // /abc/def?a=1&b=2#section
    // \______/
    //    |
    //   path
    size_t queryPos = str.find_first_of('?');
    size_t fragmentPos = str.find_first_of('#');
    size_t pathEnd = std::min(queryPos, fragmentPos);
    if (str.front() == '/') {
        this->path.clear();
        encode_string_(this->path, str.substr(0, pathEnd), "/");
    }

    // /abc/def?a=1&b=2#section
    //          \_____/ \_____/
    //             |       |
    //           query  fragment
    if (queryPos == std::string::npos && fragmentPos == std::string::npos) {
        // no query no fragment
        return;
    } else if (queryPos == std::string::npos) {
        // no query
        encode_string_(this->fragment, str.substr(pathEnd + 1));
    } else if (fragmentPos == std::string::npos) {
        // no fragment
        encode_string_(this->query, str.substr(pathEnd + 1), "=&?");
    } else {
        if (fragmentPos < queryPos) {
            // #....?....
            encode_string_(this->query, str.substr(queryPos + 1), "=&?");
            encode_string_(this->fragment, str.substr(fragmentPos + 1, queryPos - fragmentPos - 1));
        } else {
            // ?....#....
            encode_string_(this->query, str.substr(queryPos + 1, fragmentPos - queryPos - 1), "=&?");
            encode_string_(this->fragment, str.substr(fragmentPos + 1));
        }
    }
}

void Url::parse_url_() {
    // http://user:password@example.com:8080/abc/def?a=1&b=2#section
    // \__/
    //   |
    // scheme
    size_t pos = raw.find_first_of(":");
    if (pos == std::string::npos || pos + 2 >= raw.size()
            || raw.substr(pos + 1, 2) != "//") {
        throw "invalid url";
    }
    this->scheme = raw.substr(0, pos);

    // get authority and what follows
    pos += 3;
    size_t authorityEnd = std::string::npos;
    for (size_t i = pos; i < raw.size(); ++i) {
        if (raw[i] == '/' || raw[i] == '?' || raw[i] == '#') {
            authorityEnd = i; break;
        }
    }

    // http://user:password@example.com:8080/abc/def?a=1&b=2#section
    //        A                             A                      A
    //        |                             |                      |
    //       pos                       authorityEnd               end
    //        \____________  ______________/\__________  __________/
    //                     \/                          \/
    //                parseAuthority           parseAuthorityAfter

    parse_authority_(raw.substr(pos, authorityEnd - pos));
    if (authorityEnd != std::string::npos) {
        parse_authority_after_(raw.substr(authorityEnd));
    }
}


Url::Url(std::string&& url) : raw(std::move(url)) {
    parse_url_();
}

Url::Url(const std::string& url) : raw(url) {
    parse_url_();
}

Url::Url(Url&& url)
        : raw(std::move(url.raw)),
        scheme(std::move(url.scheme)),
        user(std::move(url.user)),
        password(std::move(url.password)),
        host(std::move(url.host)),
        path(std::move(url.path)),
        query(std::move(url.query)),
        fragment(std::move(url.fragment)),
        port(url.port) {
    url.port = 0;
}

void Url::parse(const std::string& url) {
    this->raw = url;
    parse_url_();
}

void Url::parse(std::string&& url) {
    this->raw = std::move(url);
    parse_url_();
}

std::string Url::authority_after() const {
    std::string result;
    result += this->path;
    if (!this->query.empty())
        result += '?' + this->query;
    if (!this->fragment.empty())
        result += '#' + this->fragment;
    return result;
}

void Url::authority_after(const std::string& authority) {
    parse_authority_after_(std::string(authority));
}

void Url::authority_after(std::string&& authority) {
    parse_authority_after_(std::move(authority));
}

std::string Url::to_string() const {
    if (this->scheme.empty()) return "";
    std::string result = this->scheme + "://";
    if (!this->user.empty()) {
        if (!this->password.empty()) {
            result += this->user + ':' + this->password + '@';
        } else {
            result += this->user + '@';
        }
    }
    result += this->host;
    if (this->port != 0 && this->port.get() != get_default_port_(this->scheme).get()) {
        result += ":" + this->port.to_string();
    }
    result += this->authority_after();
    return result;
}

}; // namespace webstab