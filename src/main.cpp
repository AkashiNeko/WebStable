#include "app/ArgsParser.h"
#include "app/Config.h"
#include "core/WebServer.h"

#include <iostream>

#define WEBSTABLE_VERSION "1.0"

int main(int argc, char* argv[]) {
    webstab::ArgsParser args(argc, argv);
    args.parse();
    webstab::Config config(args.conf_filepath());
    webstab::WebServer server(config);
    return server.exec();
}
