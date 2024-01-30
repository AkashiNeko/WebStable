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

const char* VersionText =
    "WebStable Version: " APP_VERSION_ "\n";

}

ArgsParser::ArgsParser(int argc, char** argv)
    : opt_h_(false)
    , opt_v_(false)
    , opt_c_(false)
    , conf_file_("webstable.conf")
{
    for (int i = 1; i < argc; ++i)
        args.push_back(argv[i]);
}

void ArgsParser::parse() {
    // parse arguments
    bool pre_optc = false;
    for (auto& arg : this->args) {
        size_t len = arg.size();
        if (pre_optc) {
            // The last argument contained the
            // option 'c', get config file name
            conf_file_ = arg;
            pre_optc = false;
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
                    std::cerr << "Invalid option: " << arg[i] << std::endl;
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

std::string ArgsParser::conf_filepath() {
    return std::string(conf_file_);
}

} // namespace webstab
