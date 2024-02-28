#include "app/ArgsParser.h"
#include "app/Config.h"
#include "core/WebServer.h"

int main(int argc, char* argv[]) {
    webstab::ArgsParser args(argc, argv);
    args.parse();
    webstab::Config config(args.conf_filepath());
    webstab::WebServer server(config);
    return server.exec();
}
