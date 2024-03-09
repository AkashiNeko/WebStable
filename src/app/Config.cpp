#include "Config.h"

namespace {

const char* DefaultConfName =
    "webstable.conf";

void rm_front_blank_(std::string& s) {
    if (!s.empty()) {
        size_t i = 0, sz = s.size();
        size_t offset = s.find_first_not_of(" \t");
        if (offset) {
            for (; i < sz; ++i) s[i] = s[i + offset];
            s.resize(sz - offset);
        }
    }
}

void rm_back_blank_(std::string& s) {
    if (!s.empty()) {
        size_t pos = s.find_last_not_of(" \t");
        if (pos != std::string::npos && pos != s.size() - 1)
            s.resize(pos + 1);
    }
}

inline void rm_front_back_blank_(std::string& s) {
    rm_back_blank_(s);
    rm_front_blank_(s);
}

void is_valid_path_(std::filesystem::path& file) {
    if (file.empty()) {
        // default configuration file
        file = std::filesystem::current_path()
                .append(DefaultConfName);
    } else {

        if (!std::filesystem::exists(file)) {
            std::cerr << "The configuration file or directory "
                << file << " does not exists" << std::endl;
            exit(1);
        }

        // to the simplest path
        file = std::filesystem::canonical(file);

        // if argument is a directory
        if (std::filesystem::is_directory(file))
            file.append(DefaultConfName);
    }

    if (!std::filesystem::exists(file)) {
        std::cerr << "The configuration file " << file
            << " does not exists" << std::endl;
        exit(1);
    }
}

nano::AddrPort parse_addr_port_(const std::string& str) {
    nano::AddrPort result;
    size_t pos = str.find_last_of(':');
    if (pos == std::string::npos) {
        result.addr(nano::Addr(str));
        result.port(80);
    } else {
        result.addr(nano::Addr(str.substr(0, pos)));
        result.port(nano::Port(str.substr(pos + 1)));
    }
    return result;
}

void split_keys_(const std::string& key,
        std::unordered_map<std::string, std::string>& map,
        const std::string& value) {
    std::size_t start = 0;
    std::size_t end = key.find_first_of(' ');

    while (end != std::string::npos) {
        if (end != start)
            map[key.substr(start, end - start)] = value;
        start = end + 1;
        end = key.find_first_of(' ', start);
    }

    if (start != key.size()) map[key.substr(start)] = value;
}

using SectionType = enum { Global, Server, Static, Types, Error };

SectionType parse_section_(const std::string& line, std::string_view fname, size_t line_num) {
    if (line.back() != ']') {
        std::cerr << fname << ':' << line_num
            << ": The closing bracket cannot be found." << std::endl;
        exit(3);
    }
    static std::unordered_map<std::string, SectionType> map = {
        { "[server]", Server },
        { "[static]", Static },
        { "[error]", Error },
        { "[types]", Types },
    };
    auto it = map.find(line);
    if (it == map.end()) {
        std::cerr << fname << ':' << line_num
            << ": Invalid section name." << std::endl;
        exit(4);
    }
    return it->second;
}

} // anonymous namespace

namespace webstab {

Config::Config() : server_({
    { "listen", "0.0.0.0:80" },
    { "threads_num", "16" },
    { "keepalive", "30" },
    { "poller", "epoll" },
    { "index", "index.html" },
    { "default_type", "application/octet-stream"},
    { "server_name", "WebStable" } }) {}

Config::Config(std::filesystem::path file) : Config() {
    is_valid_path_(file);

    // read the configuration file
    std::ifstream conf(file, std::ios::in);
    if (conf.fail()) {
        int err = errno;
        std::cerr << "Read configuration file "
            << file << " failed: "
            << std::strerror(err) << std::endl;
        exit(err);
    }
    std::string line;
    size_t line_num = 1;

    // current section
    SectionType cur = Global;

    // traverse each line of the file
    while (std::getline(conf, line)) {
        rm_front_back_blank_(line);
        if (line.empty() || line.front() == '#') continue;

        if (line.front() == '[') {
            // section
            cur = parse_section_(line, file.c_str(), line_num);
        } else {
            // key and value
            if (cur == Global) {
                // in global section
                std::cerr << file.c_str() << ':' << line_num
                    << ": Invalid line in global space." << std::endl;
                exit(5);
            }
            // in server/static/error section
            size_t pos = line.find_first_of('=');
            if (pos == std::string::npos) {
                // is not a k-v string
                std::cerr << file.c_str() << ':' << line_num
                    << ": This line does not contain key-value" << std::endl;
                exit(6);
            }
            // get key-value
            std::string key = line.substr(0, pos), value = line.substr(pos + 1);
            rm_back_blank_(key);
            rm_front_blank_(value);
            if (key.empty()) {
                std::cerr << file.c_str() << ':' << line_num
                    << ": Key is empty" << std::endl;
                exit(7);
            }

            // parse
            switch (cur) {
            case Server: // [server]
                this->server_[key] = value;
                break;
            case Static: // [static]
                this->static_[key] = value;
                break;
            case Error: // [error]
                split_keys_(key, this->error_, value);
                break;
            case Types: // [type]
                split_keys_(key, this->types_, value);
                break;
            default: break;
            }
        }
        ++line_num;
    }
    conf.close();
    if (this->static_path("root").empty()) {
        std::cerr << "static path: root is empty" << std::endl;
        exit(1);
    }
}

std::string Config::to_string() const {
    std::string res =  "[server]\n";
    for (const auto& [key, value] : this->server_) {
        res += '\t' + key + " = " + value + '\n';
    }
    if (!this->static_.empty()) {
        res += "\n[static]\n";
        for (const auto& [path, local] : this->static_)
            res += '\t' + path + " = " + local.c_str() + '\n';
    }
    if (!this->error_.empty()) {
        res += "\n[error]\n";
        for (const auto& [code, file] : this->error_)
            res += '\t' + code + " = " + file + '\n';
    }
    if (!this->types_.empty()) {
        res += "\n[types]\n";
        for (const auto& [name, type] : this->types_)
            res += '\t' + name + " = " + type + '\n';
    }
    return res;
}

nano::AddrPort Config::get_listen() const {
    auto it = server_.find("listen");
    if (it == server_.end())
        return nano::AddrPort({nano::Addr(), nano::Port(80)});
    return nano::AddrPort(it->second);
}

void Config::set_listen(const nano::AddrPort& addr_port) {
    server_["listen"] = addr_port.to_string();
}

std::string Config::server(const std::string& name) const {
    auto it = server_.find(name);
    if (it == server_.end()) return "";
    return it->second;
}

size_t Config::threads_num() const {
    return std::stoul(server_.at("threads_num"));
}

std::string Config::poller() const {
    return server_.at("poller");
}

std::string Config::type(const std::string& extension) const {
    auto it = types_.find(extension);
    if (it == types_.end()) {
        auto default_type = server_.find("default_type");
        if (default_type != server_.end()) return default_type->second;
        return "application/octet-stream";
    }
    return it->second;
}

std::string Config::server_name() const {
    auto it = server_.find("server_name");
    if (it == types_.end())
        return "WebStable";
    return it->second;
}

std::filesystem::path Config::static_path(std::string url_path) const {
    auto it = static_.find(url_path);
    if (it == static_.end()) return std::filesystem::path();
    return it->second;
}

size_t Config::keepalive_timeout() const {
    auto it = server_.find("keepalive");
    if (it == types_.end())
        return 30;
    return std::stoul(it->second);
}

} // namespace webstab
