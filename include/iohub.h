// iohub.h

#pragma once
#ifndef IOHUB_H
#define IOHUB_H

// C
#include <cerrno>
#include <cstring>

// C++
#include <exception>
#include <utility>
#include <queue>
#include <string>
#include <map>
#include <unordered_map>

// Linux
#include <unistd.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/epoll.h>

namespace iohub {

// except

class IOHubExcept : public std::exception {
    std::string except_msg_;
public:
    explicit IOHubExcept(const std::string& msg) : except_msg_(msg) {}
    explicit IOHubExcept(std::string&& msg) : except_msg_(std::move(msg)) {}
    virtual ~IOHubExcept() override = default;
    virtual const char* what() const noexcept override {
        return except_msg_.c_str();
    }
};

// throw exceptions
#if __cplusplus >= 201703L

template <class ExceptType = IOHubExcept, class ...Args>
inline void assert_throw(bool condition, const Args&... args) {
    if (condition) return;
    std::string s;
    ((s += args), ...);
    throw ExceptType(std::move(s));
}

#else // 201103L <= __cplusplus < 201703L

inline void append_string_(std::string&) {}

template <class T, class ...Args>
inline void append_string_(std::string& s, const T& arg, const Args&... args) {
    s += arg;
    append_string_(s, args...);
}

template <class ExceptType = IOHubExcept, class ...Args>
inline void assert_throw(bool condition, const Args&... args) {
    if (condition) return;
    std::string s;
    append_string_(s, args...);
    throw ExceptType(std::move(s));
}

#endif // __cplusplus

// pair {fd: int, event: int}
using fd_event_t = std::pair<int, int>;

enum Event {
    IOHUB_IN  = 0x01,
    IOHUB_PRI = 0x02,
    IOHUB_OUT = 0x04,
}; // Event

// PollerBase
class PollerBase {
public:
    // ctor & dtor
    PollerBase() = default;
    virtual ~PollerBase() = default;

    // uncopyable
    PollerBase(const PollerBase&) = delete;
    PollerBase& operator=(const PollerBase&) = delete;

    virtual void insert(int fd, int events) = 0;
    virtual void erase(int fd) = 0;
    virtual void modify(int fd, int events) = 0;
    virtual int get_event(int fd) const noexcept = 0;
    virtual size_t size() const noexcept = 0;
    virtual void clear() noexcept = 0;

    virtual fd_event_t wait(int timeout = -1) = 0;

    virtual bool is_open() const noexcept = 0;
    virtual void close() noexcept = 0;

}; // class PollerBase

// Select

class Select : public PollerBase {
    std::queue<fd_event_t> event_queue_;
    std::vector<unsigned char> fd_hasharr_;
    size_t max_, size_, readsz_, writesz_, exceptsz_;
    fd_set readfds_, writefds_, exceptfds_;
    bool is_open_;

public:
    Select();
    virtual ~Select() override = default;

    virtual void insert(int fd, int events) override;
    virtual void erase(int fd) override;
    virtual void modify(int fd, int events) override;
    virtual int get_event(int fd) const noexcept override;
    virtual size_t size() const noexcept override;
    virtual void clear() noexcept override;

    virtual fd_event_t wait(int timeout = -1);

    virtual bool is_open() const noexcept override;
    virtual void close() noexcept override;

}; // class Select

// Poll

class Poll : public PollerBase {
    std::queue<fd_event_t> event_queue_;
    std::unordered_map<int, int> fd_map_;
    std::vector<pollfd> pollfd_arr_;
    bool is_open_;

public:
    Poll();
    virtual ~Poll() override = default;

    virtual void insert(int fd, int events) override;
    virtual void erase(int fd) override;
    virtual void modify(int fd, int events) override;
    virtual int get_event(int fd) const noexcept override;
    virtual size_t size() const noexcept override;
    virtual void clear() noexcept override;

    virtual fd_event_t wait(int timeout = -1);

    virtual bool is_open() const noexcept override;
    virtual void close() noexcept override;

}; // class Poll

// Epoll

class Epoll : public PollerBase {
    std::queue<fd_event_t> event_queue_;
    std::unordered_map<int, int> fd_map_;
    int epoll_fd_;

public:
    Epoll();
    virtual ~Epoll() = default;

    virtual void insert(int fd, int events) override;
    virtual void erase(int fd) override;
    virtual void modify(int fd, int events) override;
    virtual int get_event(int fd) const noexcept override;
    virtual size_t size() const noexcept override;
    virtual void clear() noexcept override;

    virtual fd_event_t wait(int timeout = -1);

    virtual bool is_open() const noexcept override;
    virtual void close() noexcept override;

}; // class Epoll

} // namespace iohub

#endif // IOHUB_H