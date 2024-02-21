#include "respond.h"
#include "app/version.h"

namespace webstab {

namespace {

bool is_keep_alive_(std::string_view connection, std::string_view version) {
    if (connection == "keep-alive") return true;
    else if (connection == "close") return false;
    return version >= "HTTP/1.1";
}

void send_404_page_(nano::sock_t socket, const Config& cfg) {
    HttpRespond respond;
    respond.status_code = "404";
    respond.status_message = "Not Found";
    respond.headers["Server"] = cfg.server_name();
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
    nano::send_msg(socket, str.c_str(), str.size());
}

void send_file_(nano::sock_t socket, const std::string& version,
        const std::filesystem::path& path, const Config& cfg) {
    std::string extension = path.extension().string();
    if (!extension.empty() && extension.front() == '.')
        extension = extension.substr(1);
    std::string type = cfg.type(extension);
    HttpRespond respond;
    respond.version = version;
    respond.headers["Server"] = cfg.server_name();
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
    nano::send_msg(socket, str.c_str(), str.size());
    // std::ifstream(path, std::ios::binary);
}

} // anonymous namespace

bool reply(nano::sock_t sock, const HttpRequest& request, const Config& cfg) {
    auto path = cfg.static_path("root").append(request.relative_path());
    if (std::filesystem::is_directory(path)) {
        path.append(cfg.server("index"));
    }
    std::cout << path.c_str() << std::endl;
    if (!std::filesystem::exists(path)) {
        send_404_page_(sock, cfg);
    } else {
        send_file_(sock, request.version, path, cfg);
    }
    if (!is_keep_alive_(request.lower_header("Connection"), request.version)) {
        nano::close_socket(sock);
        printf("connection closed, fd = %d\n", sock);
        return false;
    }
    // nano::close_socket(sock);
    // printf("socket %d closed\n", sock);
    return true;
}

} // namespace webstab
