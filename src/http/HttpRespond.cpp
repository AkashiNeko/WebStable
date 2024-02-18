// http/http_respond.cpp

#include "HttpRespond.h"

namespace webstab {


HttpRespond::HttpRespond(const std::string& version, const std::string& statusCode, const std::string& statusMessage)
    : version(version), statusCode(statusCode), statusMessage(statusMessage) {}

// to string
std::string HttpRespond::toString() const {

    if (version.empty() || statusCode.empty() || statusMessage.empty())
        return "";

    std::string result = version + ' ' + statusCode + ' ' + statusMessage + "\r\n";

    for (const auto& e : headers)
        result += e.first + ": " + e.second + "\r\n";

    result += "\r\n" + body;
    return std::move(result);
}

} // namespace webstab