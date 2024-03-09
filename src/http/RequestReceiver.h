// File:     src/http/RequestReceiver.h
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
#ifndef WEBSTABLE_HTTP_REQUESTRECEIVER_H
#define WEBSTABLE_HTTP_REQUESTRECEIVER_H

// C++
#include <string>

// WebStable
#include "http/HttpRequest.h"

namespace webstab {

class RequestReceiver {

    HttpRequest& httpmsg_;

    // check
    std::string body_cache_;
    std::string head_cache_;
    std::string chunk_cache_;

    size_t body_begin_pos_ = std::string::npos;
    size_t body_begin_pos_cache_ = 0;
    size_t header_content_length_ = std::string::npos;
    size_t chunk_last_ = 0;
    size_t chunk_pos_ = 0;
    size_t chunk_base_ = 0;

    bool chunked_transfer_encoding_ = false;
    bool is_ok_ = false;
    bool head_done_ = false;

private:

    bool fill_head_(HttpRequest& request, const char* msg);

    // append chunks when 'Transfer-Encoding' is 'chunked'
    bool append_chunk_(const char* msg);
    bool append_body_(const char* msg);

public:

    RequestReceiver(HttpRequest& httpmsg);
    bool append(const char* msg);

}; // class RequestReceiver

} // namespace webstab

#endif // WEBSTABLE_HTTP_REQUESTRECEIVER_H
