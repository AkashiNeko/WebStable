// http_assembler.h

#pragma once
#ifndef __HTTP_ASSEMBLER_H__
#define __HTTP_ASSEMBLER_H__

#include <iostream>
#include <string>

#include "HttpRequest.h"
#include "HttpRespond.h"

namespace webstab {

class HttpAssembler {

    HttpRequest& httpmsg;

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

    HttpAssembler(HttpRequest& httpmsg);
    bool append(const char* msg);

};  // class HttpAssembler

}  // namespace webstab

#endif  // __HTTP_ASSEMBLER_H__