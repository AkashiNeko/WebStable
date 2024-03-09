// File:     src/file/FileCache.h
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
#ifndef WEBSTABLE_FILE_FILECACHE_H
#define WEBSTABLE_FILE_FILECACHE_H

// C++
#include <list>
#include <string>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

namespace webstab {

class FileCache {
    // types
    struct FileInfo {
        std::string filepath;
        std::time_t time;
        std::string content;
    };
    using List = std::list<FileInfo>;
    using HashMap = std::unordered_map<std::string, List::iterator>;

private:
    // LRU list and map
    List list_;
    HashMap map_;

    // locks
    std::shared_mutex rwlock_;
    std::mutex lru_mutex_;

    size_t capacity_;
    size_t size_;

public:
    explicit FileCache(size_t capacity = 100UL * 1024UL * 1024UL);

    // non-copyable
    FileCache(const FileCache&) = delete;
    FileCache& operator=(const FileCache&) = delete;

    bool get_file(const std::string& key, std::string& result) noexcept;

}; // class FileCache

} // namespace webstab

#endif // WEBSTABLE_FILE_FILECACHE_H
