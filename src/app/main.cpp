#include "ArgsParser.h"
// #include "../WebServer.h"
#include "../Config.h"

#include <iostream>

#define WEBSTABLE_VERSION "1.0"

int main(int argc, char* argv[]) {
    webstab::ArgsParser args(argc, argv);
    args.parse();
    webstab::Config config(args.conf_filepath());
    std::cout << config.to_string() << std::endl;
    // webstab::WebServer server;
    // server.load(config);
    // return server.exec();
    return 0;
}
