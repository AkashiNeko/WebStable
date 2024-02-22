#pragma once

#include "nanonet.h"
#include "app/Config.h"
#include "http/HttpRequest.h"
#include "http/HttpRespond.h"

namespace webstab {

class Responser {
    const Config& cfg_;
    const HttpRequest& request_;
    const nano::sock_t& sock_;

    void send_404_page_();
    void send_file_(const std::filesystem::path& path);

public:
    Responser(const Config& cfg,
        const HttpRequest& request,
        const nano::sock_t& sock);
    bool reply();
};


} // namespace webstab
