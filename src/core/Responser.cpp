#include "Responser.h"
#include "app/version.h"

namespace webstab {

void Responser::send_404_page_() {
    HttpRespond respond;
    respond.status_code = "404";
    respond.status_message = "Not Found";
    respond.headers["Server"] = cfg_.server_name();
    respond.headers["Content-Type"] = "text/html";
    respond.body =
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
    respond.headers["Content-Length"] = std::to_string(respond.body.size());
    std::string str = respond.to_string();
    nano::send_msg(sock_, str.c_str(), str.size());
}

void Responser::send_file_(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    if (!extension.empty() && extension.front() == '.')
        extension = extension.substr(1);
    std::string type = cfg_.type(extension);
    HttpRespond respond;
    respond.version = request_.version;
    respond.headers["Server"] = cfg_.server_name();
    respond.headers["Content-Type"] = type;
    // TODO: read file
    std::ifstream file(path, std::ios::in | std::ios_base::binary);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        respond.body = buffer.str();
        file.close();
    } else {
        // 500 Server Error
    }
    respond.headers["Content-Length"] = std::to_string(respond.body.size());

    std::string str = respond.to_string();
    nano::send_msg(sock_, str.c_str(), str.size());

}

Responser::Responser(const Config& cfg,
        const HttpRequest& request, const nano::sock_t& sock)
    : cfg_(cfg), request_(request), sock_(sock) {}

bool Responser::reply() {
    std::cout << "\033[36m" << request_.method << ' '
        << request_.path << ' ' << request_.version << "\033[0m" << std::endl;
    auto path = cfg_.static_path("root").append(request_.relative_path());
    if (std::filesystem::is_directory(path)) {
        path.append(cfg_.server("index"));
    }
    std::cout << "local: \033[33m" << path.c_str() << "\033[0m" << std::endl;
    if (!std::filesystem::exists(path)) {
        send_404_page_();
    } else {
        send_file_(path);
    }
    return false;
    return request_.keep_alive();
}

} // namespace webstab
