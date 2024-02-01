// Config.h

#pragma once
#ifndef WEBSTABLE_WEB_SERVER_H
#define WEBSTABLE_WEB_SERVER_H

// C
#include <cstring>

// C++
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <string>

// nanonet
#include <nanonet/nanonet.h>

namespace webstab {

class Config {

    std::string filename_;

    struct {
        nano::AddrPort listen;
    } server_;

    std::unordered_map<std::string, std::string> static_;
    std::map<unsigned, std::string> error_;

public:
    Config(std::filesystem::path file = "");
    std::string to_string() const;
};

} // namespace webstab

#endif // WEBSTABLE_WEB_SERVER_H
