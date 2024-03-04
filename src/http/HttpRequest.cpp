// http/http_request.cpp

#include "HttpRequest.h"

namespace webstab {

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const std::string& method,
        const Url& url, const std::string& version)
    : method(method), version(version), host(url.host()),
    path(url.authority_after()), port(url.port()),
    use_ssl(url.scheme() == "https") {}

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

}  // namespace webstab
