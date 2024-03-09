// File:     src/core/Responser.h
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
#ifndef WEBSTABLE_CORE_RESPONSER_H
#define WEBSTABLE_CORE_RESPONSER_H

// nanonet
#include "nanonet.h"

// WebStable
#include "app/Config.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"
#include "file/FileCache.h"

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
    Responser(const Config& cfg, const HttpRequest& request,
        const nano::sock_t& sock);

    bool reply();

}; // class Responser

} // namespace webstab

#endif // WEBSTABLE_CORE_RESPONSER_H
