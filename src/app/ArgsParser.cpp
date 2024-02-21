// ArgsParser.cpp

#include "ArgsParser.h"
namespace webstab {

namespace {

const char* HelpText =
    "Usage: webstable [-?hvV] [-c filename]\n"
    "Options:\n"
    "  -h,-?        : show this help\n"
    "  -v,-V        : show version\n"
    "  -c filename  : start with configuration file\n";

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

const char* VersionText =
    "* WebStable  Version: " APP_VERSION_ "\n"
    "* NanoNet    Version: " TOSTRING(__NANONET__) "\n"
    "GitHub: https://github.com/AkashiNeko/WebStable\n";

}

ArgsParser::ArgsParser(int argc, char** argv)
        : opt_h_(false), opt_v_(false), opt_c_(false) {
    for (int i = 1; i < argc; ++i)
        args.push_back(argv[i]);
}

void ArgsParser::parse() {
    // parse arguments
    bool pre_optc = false;
    for (auto& arg : this->args) {
        size_t len = arg.size();
        if (pre_optc) {
            // if the last argument is option '-c'
            // then get path of configuration file
            pre_optc = false;
            conf_file_ = arg;
        } else if (arg[0] == '-') {
            // parse option
            for (size_t i = 1; i < len; ++i) {
                switch (arg[i]) {
                case 'v': [[fallthrough]];
                case 'V':
                    opt_v_ = true;
                    break;
                case 'h': [[fallthrough]];
                case '?':
                    opt_h_ = true;
                    break;
                case 'c':
                    opt_c_ = pre_optc = true;
                    break;
                default:
                    std::cerr << "Invalid option: "
                        << arg[i] << std::endl;
                    exit(-1);
                }
            }
        } else {
            std::cerr << "Invalid argument: " << arg << std::endl;
            exit(-1);
        }
    } // parse done

    // Helo or Version, show then exit
    if (opt_v_ || opt_h_) {
        if (opt_v_) std::cout << VersionText;
        if (opt_h_) std::cout << HelpText;
        exit(0);
    }
    // OK
}

std::string_view ArgsParser::conf_filepath() {
    return conf_file_;
}

} // namespace webstab
