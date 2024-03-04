// http/http_respond.cpp

#include "HttpRespond.h"

namespace webstab {


HttpRespond::HttpRespond(const std::string& version,
        const std::string& statusCode,
        const std::string& statusMessage)
    : version(version),
    status_code(statusCode),
    status_message(statusMessage) {}

// to string
std::string HttpRespond::to_string() const {

    if (version.empty() || status_code.empty() || status_message.empty())
        return "";

    std::string result = version + ' ' + status_code + ' ' + status_message + "\r\n";

    for (const auto& e : headers)
        result += e.first + ": " + e.second + "\r\n";

    result += "\r\n" + body;
    return result;
}

} // namespace webstab