// File:     src/app/ArgsParser.h
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

#pragma once
#ifndef WEBSTABLE_APP_ARGSPARSER_H
#define WEBSTABLE_APP_ARGSPARSER_H

// C++
#include <vector>
#include <string>

namespace webstab {

class ArgsParser {
    std::vector<std::string_view> args;
    bool opt_h_, opt_v_, opt_c_;
    std::string_view conf_file_;

public:
    ArgsParser(int argc, char** argv);
    void parse();
    std::string_view conf_filepath();

}; // class ArgParser

} // namespace webstab

#endif // WEBSTABLE_APP_ARGSPARSER_H
