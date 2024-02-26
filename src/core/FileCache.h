#pragma once

#include <list>
#include <string>
#include <unordered_map>
#include <utility>
#include <filesystem>
#include <fstream>
#include <functional>
#include <shared_mutex>
#include <mutex>
#include <ctime>
#include <sys/stat.h>

namespace webstab {

class FileCache {
    struct FileInfo {
        std::string filepath;
        std::time_t time;
        std::string content;
    };
    using List = std::list<FileInfo>;
    using HashMap = std::unordered_map<std::string, List::iterator>;

    List list_;
    HashMap map_;
    std::shared_mutex rwlock_;
    std::mutex lru_mutex_;
    size_t capacity_;
    size_t size_;

public:
    FileCache(size_t capacity = 100UL * 1024UL * 1024UL)
        : capacity_(capacity), size_(0UL) {}

    bool get_file(const std::string& key, std::string& result) noexcept;
};

} // namespace webstab
