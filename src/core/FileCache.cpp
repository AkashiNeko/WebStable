#include "FileCache.h"

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

// get_file(...) {
//     rwlock.lock(r);
//     if (cache hit && cache is latest) {
//         mutex.lock();
//         * update LRU list and hashmap
//         mutex.unlock();
//         * read cache
//         rwlock.unlock(r);
//     } else {
//         rwlock.unlock(r);
//         rwlock.lock(w);
//         * read file, update cache
//         mutex.lock();
//         * update LRU list and hashmap
//         mutex.unlock();
//         rwlock.unlock(w);
//     }
// }

bool FileCache::get_file(const std::string& path, std::string& result) noexcept {
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