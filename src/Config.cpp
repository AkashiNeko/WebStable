#include "Config.h"

namespace {

const char* DefaultConfName =
    "webstable.conf";

const char* DefaultConfText =
    "[server]"                          "\n"
    "    listen = 0.0.0.0:80"           "\n"
    ""                                  "\n"
    "[static]"                          "\n"
    "    / = www/"                      "\n"
    ""                                  "\n"
    "[error]"                           "\n"
    "    404 = /404.html"               "\n"
    "    500 502 503 504 = /50x.html"   "\n";

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
        result.set_addr(str);
        result.set_port(80);
    } else {
        result.set_addr(str.substr(0, pos));
        result.set_port(str.substr(pos + 1));
    }
    return result;
}

void split_digit_str_(const std::string& key,
                    std::map<unsigned, std::string>& map,
                    const std::string& value) {
    std::size_t start = 0;
    std::size_t end = key.find_first_of(' ');

    while (end != std::string::npos) {
        if (end != start)
            map.insert({std::stoul(key.substr(start, end - start)), value});
        start = end + 1;
        end = key.find_first_of(' ', start);
    }

    if (start != key.size()) map.insert({std::stoul(key.substr(start)), value});
}

} // namespace

namespace webstab {

Config::Config()
    : srv_listen_("0.0.0.0:80")
    , srv_poller_("epoll")
{
}

Config::Config(std::filesystem::path file)
    : Config()
{
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
    enum { Global, Server, Static, Error } cur = Global;

    // traverse each line of the file
    while (std::getline(conf, line)) {
        rm_front_back_blank_(line);
        if (line.empty() || line.front() == '#') continue;

        if (line.front() == '[') {
            // section
            if (line.back() != ']') {
                std::cerr << file.c_str() << ':' << line_num
                    << ": The closing bracket cannot be found." << std::endl;
                exit(3);
            }
            if (line == "[server]") {
                cur = Server;
            } else if (line == "[static]") {
                cur = Static;
            } else if (line == "[error]") {
                cur = Error;
            } else {
                std::cerr << file.c_str() << ':' << line_num
                    << ": Invalid section name." << std::endl;
                exit(4);
            }

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
            // get k-v
            std::string key = line.substr(0, pos), value = line.substr(pos + 1);
            rm_back_blank_(key);
            rm_front_blank_(value);
            if (key.empty()) {
                std::cerr << file.c_str() << ':' << line_num
                    << ": Key is empty" << std::endl;
                exit(7);
            }

            // parse
            if (cur == Server) {
                if (key == "listen") {
                    try {
                        this->srv_listen_ = parse_addr_port_(value);
                    } catch (const std::exception& e) {
                        std::cerr << file.c_str() << ':' << line_num
                            << ": Parse listen address failed: " << e.what() << std::endl;
                        exit(8);
                    }
                } else {
                    std::cerr << file.c_str() << ':' << line_num
                        << ": Unknown config key: \"" << key << '\"' << std::endl;
                    exit(9);
                }
            } else if (cur == Static) {
                this->static_.insert({key, value});
            } else { // Error
                try {
                    split_digit_str_(key, this->error_, value);
                } catch (...) {
                    std::cerr << file.c_str() << ':' << line_num
                        << ": Key is not a number." << std::endl;
                    exit(10);
                }
            }
        }
        ++line_num;
    }
    conf.close();
}

std::string Config::to_string() const {
    std::string res =  "[server]\n"
        "\tlisten = " + srv_listen_.to_string() + '\n';
    if (!static_.empty()) {
        res += "\n[static]\n";
        for (auto& [k, v] : static_)
            res += '\t' + k + " = " + v + '\n';
    }
    if (!error_.empty()) {
        res += "\n[error]\n";
        for (auto& [k, v] : error_)
            res += '\t' + std::to_string(k) + " = " + v + '\n';
    }
    return res;
}

nano::AddrPort Config::get_listen() const {
    return srv_listen_;
}

void Config::set_listen(const nano::AddrPort& addr_port) {
    srv_listen_ = addr_port;
}

} // namespace webstab
