#include "cli.hpp"

#include <iostream>
#include <vector>

static bool is_flag(const std::string& s, const char* shortf, const char* longf) {
    return s == shortf || s == longf;
}

CLIOptions parse_cli(int argc, char** argv) {
    CLIOptions opts;
    std::vector<std::string> args(argv + 1, argv + argc);
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& a = args[i];
        if (is_flag(a, "-h", "--help")) {
            opts.show_help = true;
            break;
        } else if (is_flag(a, "-i", "--input")) {
            if (i + 1 < args.size()) {
                opts.input_dir = args[++i];
            } else {
                std::cerr << "Missing value for " << a << "\n";
            }
        } else if (is_flag(a, "-o", "--output")) {
            if (i + 1 < args.size()) {
                opts.output_dir = args[++i];
            } else {
                std::cerr << "Missing value for " << a << "\n";
            }
        } else if (is_flag(a, "-f", "--file")) {
            if (i + 1 < args.size()) {
                opts.output_filename = args[++i];
            } else {
                std::cerr << "Missing value for " << a << "\n";
            }
        } else {
            std::cerr << "Unknown argument: " << a << "\n";
        }
    }
    return opts;
}

void print_help(const char* prog) {
    std::cout << "Usage: " << (prog ? prog : "panorama")
              << " [-i|--input <images_dir>] [-o|--output <out_dir>] [-f|--file <out_file>]" << '\n'
              << "\n"
              << "Options:\n"
              << "  -i, --input   Directory containing input images (default: ./images)\n"
              << "  -o, --output  Directory to write panorama (default: ./output)\n"
              << "  -f, --file    Output filename, e.g. panorama.jpg (default: panorama.jpg)\n"
              << "  -h, --help    Show this help and exit\n"
              << std::endl;
}
