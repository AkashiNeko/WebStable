// except.h

#pragma once
#ifndef WEBSTABLE_EXCEPT_H
#define WEBSTABLE_EXCEPT_H

// C
#include <cerrno>
#include <cstring>

// C++
#include <string>
#include <exception>

namespace webstab {

class WebStableExcept : public std::exception {
    std::string except_msg_;
public:
    explicit WebStableExcept(const std::string& msg) : except_msg_(msg) {}
    explicit WebStableExcept(std::string&& msg) : except_msg_(std::move(msg)) {}
    virtual ~WebStableExcept() override = default;
    virtual const char* what() const noexcept override {
        return except_msg_.c_str();
    }
};

// throw exceptions
#if __cplusplus >= 201703L

template <class ExceptType, class ...Args>
inline void throw_except_(const Args&... args) {
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

template <class ExceptType, class ...Args>
inline void throw_except_(const Args&... args) {
    std::string s;
    append_string_(s, args...);
    throw ExceptType(std::move(s));
}

#endif // __cplusplus

#define assert_throw(condition, ...)          \
(static_cast<bool>(condition) ? void(0) :     \
throw_except_<WebStableExcept>(__VA_ARGS__)); \

} // namespace webstab

#endif // WEBSTABLE_EXCEPT_H
