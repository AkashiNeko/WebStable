#pragma once

#include "nanonet.h"

#include "app/Config.h"
#include "http/HttpRequest.h"
#include "http/HttpRespond.h"

#include "FileCache.h"

namespace webstab {

class Responser {
    const Config& cfg_;
    const HttpRequest& request_;
    const nano::sock_t& sock_;
    FileCache cache_;

    bool send_default_404_page_();
    bool send_respond_(const std::filesystem::path& path,
        const std::string& body);

public:
    Responser(const Config& cfg,
        const HttpRequest& request,
        const nano::sock_t& sock);
    bool reply();
};


} // namespace webstab
