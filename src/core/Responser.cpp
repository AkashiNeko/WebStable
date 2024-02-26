#include "Responser.h"
#include "app/version.h"

namespace webstab {

namespace {

constexpr char default_404_page[] =
    "<html>"
        "<head>"
            "<title>404 Not Found</title>"
        "</head>"
        "<body>"
            "<center><h1>404 Not Found</h1></center>"
            "<hr>"
            "<center>WebStable/" TOSTRING(WEBSTABLE_VERSION) "</center>"
        "</body>"
    "</html>\n";

constexpr size_t default_404_page_size = sizeof(default_404_page) - 1;

} // anonymous namespace

void Responser::send_default_404_page_() {
    HttpRespond respond;
    respond.status_code = "404";
    respond.status_message = "Not Found";
    respond.headers["Server"] = cfg_.server_name();
    respond.headers["Content-Type"] = "text/html";
    respond.headers["Content-Length"] = std::to_string(default_404_page_size);
    std::string str = respond.to_string();
    nano::send_msg(sock_, str.c_str(), str.size());
    nano::send_msg(sock_, default_404_page, default_404_page_size);
}

void Responser::send_respond_(const std::filesystem::path& path,
        const std::string& body) {
    std::string extension = path.extension().string();
    if (!extension.empty() && extension.front() == '.')
        extension = extension.substr(1);
    std::string type = cfg_.type(extension);
    HttpRespond respond;
    respond.version = request_.version;
    respond.headers["Server"] = cfg_.server_name();
    respond.headers["Content-Type"] = type;
    respond.headers["Content-Length"] = std::to_string(body.size());
    std::string str = respond.to_string();
    nano::send_msg(sock_, str.c_str(), str.size());
    nano::send_msg(sock_, body.c_str(), body.size());
}

Responser::Responser(const Config& cfg,
        const HttpRequest& request, const nano::sock_t& sock)
    : cfg_(cfg), request_(request), sock_(sock) {}

bool Responser::reply() {
    std::cout << "\033[36m" << request_.method << ' '
        << request_.path << ' ' << request_.version << "\033[0m" << std::endl;
    auto path = cfg_.static_path("root").append(request_.relative_path());
    if (std::filesystem::is_directory(path))
        path.append(cfg_.server("index"));
    std::string file_content;
    std::cout << "local: \033[33m" << path.c_str() << "\033[0m" << std::endl;
    if (!cache_.get_file(path.string(), file_content)) {
        send_default_404_page_();
        std::cout << "404 Not Found" << std::endl;
    } else {
        send_respond_(path, file_content);
    }
    return false;
    // return request_.keep_alive();
}

} // namespace webstab
