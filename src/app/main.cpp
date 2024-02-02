#include "ArgsParser.h"
#include "../WebServer.h"
#include "../Config.h"

#include <iostream>

#define WEBSTABLE_VERSION "1.0"

int main(int argc, char* argv[]) {
    webstab::ArgsParser args(argc, argv);
    args.parse();
    webstab::Config config(args.conf_filepath());
    webstab::WebServer server(config);
    // server.load(config);
    // return server.exec();
    return 0;
}
