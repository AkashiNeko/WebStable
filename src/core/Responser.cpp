#include "Responser.h"
#include "app/version.h"

namespace webstab {

namespace {

constexpr char DEFAULT_404_PAGE[] =
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

constexpr size_t DEFAULT_404_PAGE_LENGTH = sizeof(DEFAULT_404_PAGE) - 1;

} // anonymous namespace

bool Responser::send_default_404_page_() {
    HttpRespond respond;
    respond.status_code = "404";
    respond.status_message = "Not Found";
    respond.headers["Server"] = cfg_.server_name();
    respond.headers["Content-Type"] = "text/html";
    respond.headers["Content-Length"] = std::to_string(DEFAULT_404_PAGE_LENGTH);
    std::string str = respond.to_string();
    try {
        nano::send_msg(sock_, str.c_str(), str.size(), MSG_NOSIGNAL);
        nano::send_msg(sock_, DEFAULT_404_PAGE, DEFAULT_404_PAGE_LENGTH, MSG_NOSIGNAL);
    } catch (const nano::NanoExcept& e) {
        return false;
    }
    return true;
}

bool Responser::send_respond_(const std::filesystem::path& path,
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
    try {
        nano::send_msg(sock_, str.c_str(), str.size(), MSG_NOSIGNAL);
        nano::send_msg(sock_, body.c_str(), body.size(), MSG_NOSIGNAL);
    } catch (const nano::NanoExcept& e) {
        return false;
    }
    return true;
}

Responser::Responser(const Config& cfg,
        const HttpRequest& request, const nano::sock_t& sock)
    : cfg_(cfg), request_(request), sock_(sock) {}

bool Responser::reply() {
    // std::cout << "\033[36m" << request_.method << ' '
        // << request_.path << ' ' << request_.version << "\033[0m" << std::endl;
    auto path = cfg_.static_path("root").append(request_.relative_path());
    if (std::filesystem::is_directory(path))
        path.append(cfg_.server("index"));
    std::string file_content;
    // std::cout << "local: \033[33m" << path.c_str() << "\033[0m" << std::endl;
    bool send_success = cache_.get_file(path.string(), file_content)
        ? send_respond_(path, file_content) // get file success
        : send_default_404_page_();         // get file failed
    return send_success && request_.keep_alive();
}

} // namespace webstab

