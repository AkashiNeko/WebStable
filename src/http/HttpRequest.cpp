// http/http_request.cpp

#include "HttpRequest.h"

namespace webstab {

HttpRequest::HttpRequest() :path("/"), version("HTTP/1.0") {
    this->headers["Accept"] = "*/*";
    this->headers["Host"] = host;
    this->headers["User-Agent"] = "Nanonet";
}

HttpRequest::HttpRequest(const std::string& method,
    const Url& url, const std::string& version) :version(version) {
    // set request-URI
    this->host = url.host();
    this->path = url.authority_after();
    this->port = url.port();
    this->useSSL = url.scheme() == "https";

    // set request method
    for (auto& ch : method) {
        // method to upper
        this->method.push_back(std::toupper(ch));
    }

    // set request headers

}

std::string HttpRequest::to_string() const {
    std::string request = method + ' ' + path + ' ' + version + "\r\n";

    for (const auto& e : headers)
        request += e.first + ": " + e.second + "\r\n";

    request += "\r\n" + body;
    return std::move(request);
}

}  // namespace webstab