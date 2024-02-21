#pragma once

#include "nanonet.h"
#include "app/Config.h"
#include "http/HttpRequest.h"
#include "http/HttpRespond.h"

namespace webstab {

bool reply(nano::sock_t sock, const HttpRequest& request, const Config& cfg);

} // namespace webstab
