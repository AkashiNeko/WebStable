// Config.h

#pragma once
#ifndef WEBSTABLE_CONFIG_H
#define WEBSTABLE_CONFIG_H

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

// NanoNet
#include "nanonet.h"

namespace webstab {

class Config {

    nano::AddrPort srv_listen_;
    std::string srv_poller_;
    std::unordered_map<std::string, std::string> static_;
    std::unordered_map<std::string, std::string> error_;
    std::unordered_map<std::string, std::string> types_;

public:
    Config();
    Config(std::filesystem::path file);
    std::string to_string() const;

    nano::AddrPort get_listen() const;
    void set_listen(const nano::AddrPort& addr_port);
};

} // namespace webstab

#endif // WEBSTABLE_CONFIG_H
