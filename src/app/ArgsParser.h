// ArgsParserImpl.h

#pragma once
#ifndef WEBSTABLE_ARGS_PARSER_H
#define WEBSTABLE_ARGS_PARSER_H

#include "ArgsParser.h"
#include "nanonet.h"

#include <vector>
#include <string>
#include <iostream>

namespace webstab {

class ArgsParser {
    std::vector<std::string_view> args;
    bool opt_h_, opt_v_, opt_c_;
    std::string_view conf_file_;

public:
    ArgsParser(int argc, char** argv);
    void parse();
    std::string_view conf_filepath();
};

} // namespace webstab

#endif // WEBSTABLE_ARGS_PARSER_H
