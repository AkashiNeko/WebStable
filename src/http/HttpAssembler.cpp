// http/http_assembler.cpp

#include "HttpAssembler.h"

namespace webstab {

// converts hex text to digit, on error, -1 is returned
static size_t hex2digit(const std::string& hexStr) {
    try {
        return (size_t)std::stol(hexStr, nullptr, 16);
    } catch (const std::exception& e) {
        return (size_t)std::string::npos;
    }
}

bool HttpAssembler::_fillHead(HttpRequest& request, const char* msg) {
    headCache += msg;
    bodyStart = headCache.find("\r\n\r\n", bodyStartCache);
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
    if (bodyStart != std::string::npos) {

        // set pos of body start
        bodyStart += 4;

        // separate the head (line, headders)
        bodyCache += headCache.substr(bodyStart);
        headCache.resize(bodyStart - 2);

        // get line 'METHOD PATH VERSION\r\n'
        size_t pos = headCache.find("\r\n");

        // get METHOD: GET/POST/PUT...
        size_t methodEnd = headCache.find(' ', 0);
        request.setMethod(headCache.substr(0, methodEnd));

        // get PATH: /...
        size_t pathEnd = headCache.find(' ', methodEnd + 1);
        request.setPath(headCache.substr(methodEnd + 1, pathEnd - methodEnd - 1));

        // get VERSION: HTTP/1.0, HTTP/1.1...
        request.setVersion(headCache.substr(pathEnd + 1, pos - pathEnd - 1));

        // next line
        pos += 2;

        // get headers
        while (pos < headCache.size()) {
            size_t beginLine = pos;
            size_t endLine = headCache.find("\r\n", beginLine);
            size_t colon = headCache.find(": ", beginLine);
            if (colon > endLine) continue;
            request.setHeader(
                headCache.substr(beginLine, colon - beginLine),
                headCache.substr(colon + 2, endLine - colon - 2)
            );
            pos = endLine + 2;
        }
        // set args
        const auto& headers = request.getHeaders();
        auto it = headers.find("Content-Length");
        if (it != headers.end()) {
            headerContentLength = std::stoi(it->second);
        } else {
            it = headers.find("Transfer-Encoding");
            if (it != headers.end() && it->second == "chunked")
                chunkedTransferEncoding = true;
        }
        // OK
        this->headDone = true;
        // fill body
        bool ret = _appendBody(bodyCache.c_str());
        bodyCache.clear();
        if (request.getMethod() == "GET")
            return isOK = true;
        return ret;
    }
    // cannot found '\r\n\r\n', set find start pos to size - 4
    bodyStartCache = headCache.size() - 4;
    if (bodyStartCache < 0)
        bodyStartCache = 0;
    return false;
}

// append chunks when 'Transfer-Encoding' is 'chunked'

bool HttpAssembler::_appendChunk(const char* msg) {
    chunkCache += msg;
    size_t length = chunkCache.size();
    while (chunkBase < length) {
        if (chunkLast == 0) {
            if ((chunkPos = chunkCache.find("\r\n", chunkBase)) != std::string::npos) {
                size_t chunkLength = hex2digit(
                    chunkCache.substr(chunkBase, chunkPos - chunkBase));
                if (chunkLength == 0) {
                    chunkCache.clear();
                    return isOK = true;
                }
                chunkLast = chunkLength;
                chunkBase = chunkPos += 2;
                if (chunkBase >= length)
                    return false;
            } else {
                return false;
            }
        } else {
            size_t appendLength = length - chunkBase;
            if (appendLength <= chunkLast) {
                httpmsg.appendText(chunkCache.substr(chunkBase, appendLength));
                chunkLast -= appendLength;
                chunkBase = chunkPos = length;
                return false;
            } else {
                httpmsg.appendText(chunkCache.substr(chunkBase, chunkLast));
                chunkBase += chunkLast + 2;
                chunkLast = 0;
            }
            // clear cache
            if (chunkBase == chunkCache.size()) {
                chunkCache.clear();
                chunkBase = 0;
            }
        }
    }
    return false;
}


bool HttpAssembler::_appendBody(const char* msg) {
    // when 'Transfer-Encoding' is 'chunked'
    if (chunkedTransferEncoding)
        return _appendChunk(msg);
    // append to body
    httpmsg.appendText(msg);
    // when 'Content-Length' is set
    if (headerContentLength != std::string::npos) {
        if (httpmsg.size() >= headerContentLength) {
            // receive done. truncate
            httpmsg.getText().resize(headerContentLength);
            return isOK = true;
        } else {
            // continue
            return false;
        }
    }
    return false;
}


HttpAssembler::HttpAssembler(HttpRequest& httpmsg) :httpmsg(httpmsg) {}


bool HttpAssembler::append(const char* msg) {
    if (isOK)
        return true;

    if (headDone) {
        return _appendBody(msg);
    } else {
        return _fillHead(httpmsg, msg);
    }
}

}  // namespace webstab