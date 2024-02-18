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
    std::string bodyCache;
    std::string headCache;
    std::string chunkCache;

    size_t bodyStart = std::string::npos;
    size_t bodyStartCache = 0;
    size_t headerContentLength = std::string::npos;
    size_t chunkLast = 0;
    size_t chunkPos = 0;
    size_t chunkBase = 0;

    bool chunkedTransferEncoding = false;
    bool isOK = false;
    bool headDone = false;

private:

    bool _fillHead(HttpRequest& request, const char* msg);

    // append chunks when 'Transfer-Encoding' is 'chunked'
    bool _appendChunk(const char* msg);
    bool _appendBody(const char* msg);

public:

    HttpAssembler(HttpRequest& httpmsg);
    bool append(const char* msg);

};  // class HttpAssembler

}  // namespace webstab

#endif  // __HTTP_ASSEMBLER_H__