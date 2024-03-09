// File:     src/file/FileCache.cpp
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

#include "FileCache.h"

// C
#include <ctime>

// C++
#include <filesystem>
#include <fstream>
#include <functional>

// Linux
#include <sys/stat.h>

namespace webstab {

namespace {

std::time_t file_update_time(const char* path) {
    struct stat file_stat {};
    if (::stat(path, &file_stat) == -1)
        throw 404;
    return static_cast<std::time_t>(file_stat.st_mtime);
}

bool is_latest(const std::string& path, std::time_t time) {
    std::time_t ftime = file_update_time(path.c_str());
    return ftime < time;
}

long int read_file(const std::string& path, std::string& content) {
    try {
        std::ifstream file(path, std::ios::binary);
        size_t fsize = std::filesystem::file_size(path);
        if (!file.is_open()) return -1L;
        content.resize(fsize);
        file.read(&(content[0]), fsize);
        file.close();
        return fsize;
    } catch (...) {
        return -1L;
    }
}

} // anonymous namespace

FileCache::FileCache(size_t capacity) : capacity_(capacity), size_(0UL) {}

bool FileCache::get_file(
        const std::string& path,
        std::string& result) noexcept {
    rwlock_.lock_shared(); // rwlock lock read
    auto map_it = map_.find(path);
    if (map_it != map_.end() && is_latest(path, map_it->second->time)) {
        // cache hit and cache is latest
        // TODO: read cache
        lru_mutex_.lock(); // mutex lock

        // update list and hashmap
        if (map_it->second != list_.begin()) {
            list_.emplace_front(std::move(*map_it->second));
            list_.erase(map_it->second);
            map_it->second = list_.begin();
        }

        // get content
        const std::string& content = map_it->second->content;

        lru_mutex_.unlock(); // mutex unlock

        // read cache
        result = content; // copy content from cache

        rwlock_.unlock_shared(); // rwlock unlock read
    } else {
        // cache miss or cache is not latest
        // TODO: update cache
        rwlock_.unlock_shared(); // rwlock unlock read
        rwlock_.lock(); // rwlock lock write

        // read file
        FileInfo fi {};
        fi.filepath = path;
        fi.time = std::time(nullptr);
        long int fsize = read_file(path, fi.content);
        if (fsize == -1) {
            // read failed
            rwlock_.unlock();
            return false;
        }

        // return result
        result = fi.content;

        lru_mutex_.lock(); // mutex lock
        rwlock_.unlock(); // rwlock unlock write
        list_.emplace_front(std::move(fi));
        map_.emplace(path, list_.begin());
        size_ += fsize;
        while (size_ > capacity_) {
            const FileInfo& del = list_.back();
            size_ -= del.content.size();
            map_.erase(del.filepath);
            list_.pop_back();
        }
        lru_mutex_.unlock(); // mutex unlock
    }
    return true;
}

} // namespace webstab
