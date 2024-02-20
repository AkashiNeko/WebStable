// http/http_assembler.cpp

#include "HttpAssembler.h"

namespace webstab {

namespace {

// converts hex text to digit, on error, -1 is returned
size_t hex_str_to_dec_(const std::string& hexStr) {
    try {
        return static_cast<size_t>(std::stol(hexStr, nullptr, 16));
    } catch (const std::exception& e) {
        return std::string::npos;
    }
}

} // anonymous namespace

bool HttpAssembler::fill_head_(HttpRequest& request, const char* msg) {
    head_cache_ += msg;
    body_begin_pos_ = head_cache_.find("\r\n\r\n", body_begin_pos_cache_);
    // +------------------------------+
    // | http request message example |
    // +------------------------------+
    // | POST / HTTP/1.0\r\n          |
    // | Accept: */*\r\n              |
    // | Host: 127.0.0.1\r\n          |
    // | User-Agent: Mozilla/5.0\r\n  |
    // | Content-Length: 4\r\n        |
    // | \r\n                         |
    // | text                         |
    // +------------------------------+
    // if header received in its entirety
    if (body_begin_pos_ != std::string::npos) {

        // set pos of body start
        body_begin_pos_ += 4;

        // separate the head (line, headders)
        body_cache_ += head_cache_.substr(body_begin_pos_);
        head_cache_.resize(body_begin_pos_ - 2);

        // get line 'METHOD PATH VERSION\r\n'
        size_t pos = head_cache_.find("\r\n");

        // get METHOD: GET/POST/PUT...
        size_t methodEnd = head_cache_.find(' ', 0);
        request.method = head_cache_.substr(0, methodEnd);

        // get PATH: /...
        size_t pathEnd = head_cache_.find(' ', methodEnd + 1);
        request.path = head_cache_.substr(methodEnd + 1, pathEnd - methodEnd - 1);

        // get VERSION: HTTP/1.0, HTTP/1.1...
        request.version = head_cache_.substr(pathEnd + 1, pos - pathEnd - 1);

        // next line
        pos += 2;

        // get headers
        while (pos < head_cache_.size()) {
            size_t beginLine = pos;
            size_t endLine = head_cache_.find("\r\n", beginLine);
            size_t colon = head_cache_.find(": ", beginLine);
            if (colon > endLine) continue;
            request.headers.insert({
                head_cache_.substr(beginLine, colon - beginLine),
                head_cache_.substr(colon + 2, endLine - colon - 2)
            });
            pos = endLine + 2;
        }
        // set args
        auto it = request.headers.find("Content-Length");
        if (it != request.headers.end()) {
            header_content_length_ = std::stoi(it->second);
        } else {
            it = request.headers.find("Transfer-Encoding");
            if (it != request.headers.end() && it->second == "chunked")
                chunked_transfer_encoding_ = true;
        }
        // OK
        this->head_done_ = true;
        // fill body
        bool ret = append_body_(body_cache_.c_str());
        body_cache_.clear();
        if (request.method == "GET")
            return is_ok_ = true;
        return ret;
    }
    // cannot found '\r\n\r\n', set find start pos to size - 4
    body_begin_pos_cache_ = head_cache_.size() - 4;
    if (body_begin_pos_cache_ < 0)
        body_begin_pos_cache_ = 0;
    return false;
}

// append chunks when 'Transfer-Encoding' is 'chunked'

bool HttpAssembler::append_chunk_(const char* msg) {
    chunk_cache_ += msg;
    size_t length = chunk_cache_.size();
    while (chunk_base_ < length) {
        if (chunk_last_ == 0) {
            if ((chunk_pos_ = chunk_cache_.find("\r\n", chunk_base_)) != std::string::npos) {
                size_t chunkLength = hex_str_to_dec_(
                    chunk_cache_.substr(chunk_base_, chunk_pos_ - chunk_base_));
                if (chunkLength == 0) {
                    chunk_cache_.clear();
                    return is_ok_ = true;
                }
                chunk_last_ = chunkLength;
                chunk_base_ = chunk_pos_ += 2;
                if (chunk_base_ >= length)
                    return false;
            } else {
                return false;
            }
        } else {
            size_t append_length = length - chunk_base_;
            if (append_length <= chunk_last_) {
                httpmsg.body += chunk_cache_.substr(chunk_base_, append_length);
                chunk_last_ -= append_length;
                chunk_base_ = chunk_pos_ = length;
                return false;
            } else {
                httpmsg.body += chunk_cache_.substr(chunk_base_, chunk_last_);
                chunk_base_ += chunk_last_ + 2;
                chunk_last_ = 0;
            }
            // clear cache
            if (chunk_base_ == chunk_cache_.size()) {
                chunk_cache_.clear();
                chunk_base_ = 0;
            }
        }
    }
    return false;
}


bool HttpAssembler::append_body_(const char* msg) {
    // when 'Transfer-Encoding' is 'chunked'
    if (chunked_transfer_encoding_)
        return append_chunk_(msg);
    // append to body
    httpmsg.body += msg;
    // when 'Content-Length' is set
    if (header_content_length_ != std::string::npos) {
        if (httpmsg.body.size() >= header_content_length_) {
            // receive done. truncate
            httpmsg.body.resize(header_content_length_);
            return is_ok_ = true;
        } else {
            // continue
            return false;
        }
    }
    return false;
}


HttpAssembler::HttpAssembler(HttpRequest& httpmsg) :httpmsg(httpmsg) {}


bool HttpAssembler::append(const char* msg) {
    if (is_ok_)
        return true;

    if (head_done_) {
        return append_body_(msg);
    } else {
        return fill_head_(httpmsg, msg);
    }
}

}  // namespace webstab