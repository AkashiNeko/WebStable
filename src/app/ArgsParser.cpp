// File:     src/app/ArgsParser.cpp
// Author:   AkashiNeko
// Project:  WebStable
// Github:   https://github.com/AkashiNeko/WebStable/

/* Copyright (c) 2024 AkashiNeko
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ArgsParser.h"

// C++
#include <iostream>

// nanonet
#include "nanonet.h"

// WebStable
#include "app/version.h"

namespace webstab {

namespace {

const char* HelpText =
    "Usage: webstable [-?hvV] [-c filename]\n"
    "Options:\n"
    "  -h,-?        : show this help\n"
    "  -v,-V        : show version\n"
    "  -c filename  : start with configuration file\n";

const char* VersionText =
    "* WebStable  Version: " TOSTRING(WEBSTABLE_VERSION) "\n"
    "* NanoNet    Version: " TOSTRING(NANONET_VERSION) "\n"
    "GitHub: https://github.com/AkashiNeko/WebStable\n";

}

ArgsParser::ArgsParser(int argc, char** argv)
        : opt_h_(false), opt_v_(false), opt_c_(false) {
    for (int i = 1; i < argc; ++i)
        args.push_back(argv[i]);
}

void ArgsParser::parse() {
    // parse arguments
    bool pre_optc = false;
    for (auto& arg : this->args) {
        size_t len = arg.size();
        if (pre_optc) {
            // if the last argument is option '-c'
            // then get path of configuration file
            pre_optc = false;
            conf_file_ = arg;
        } else if (arg[0] == '-') {
            // parse option
            for (size_t i = 1; i < len; ++i) {
                switch (arg[i]) {
                case 'v': [[fallthrough]];
                case 'V':
                    opt_v_ = true;
                    break;
                case 'h': [[fallthrough]];
                case '?':
                    opt_h_ = true;
                    break;
                case 'c':
                    opt_c_ = pre_optc = true;
                    break;
                default:
                    std::cerr << "Invalid option: "
                        << arg[i] << std::endl;
                    exit(-1);
                }
            }
        } else {
            std::cerr << "Invalid argument: " << arg << std::endl;
            exit(-1);
        }
    } // parse done

    // Helo or Version, show then exit
    if (opt_v_ || opt_h_) {
        if (opt_v_) std::cout << VersionText;
        if (opt_h_) std::cout << HelpText;
        exit(0);
    }
    // OK
}

std::string_view ArgsParser::conf_filepath() {
    return conf_file_;
}

} // namespace webstab
