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
        } else if (a == "--top-match-only") {
            opts.top_match_only = true;
        } else if (a == "--max-dim") {
            if (i + 1 < args.size()) {
                try {
                    opts.max_dim = std::stoi(args[++i]);
                } catch (...) {
                    std::cerr << "Invalid integer for --max-dim\n";
                }
            } else {
                std::cerr << "Missing value for --max-dim\n";
            }
        } else if (a == "--mode") {
            if (i + 1 < args.size()) {
                opts.mode = args[++i];
                if (opts.mode != "panorama" && opts.mode != "scans") {
                    std::cerr << "Invalid value for --mode (use 'panorama' or 'scans')\n";
                    opts.mode = "panorama";
                }
            } else {
                std::cerr << "Missing value for --mode\n";
            }
        } else {
            std::cerr << "Unknown argument: " << a << "\n";
        }
    }
    return opts;
}

void print_help(const char* prog) {
    std::cout << "Usage: " << (prog ? prog : "panorama")
              << " [-i|--input <images_dir>] [-o|--output <out_dir>] [-f|--file <out_file>] [--top-match-only] [--max-dim N] [--mode panorama|scans]" << '\n'
              << "\n"
              << "Options:\n"
              << "  -i, --input   Directory containing input images (default: ./images)\n"
              << "  -o, --output  Directory to write panorama (default: ./output)\n"
              << "  -f, --file    Output filename, e.g. panorama.jpg (default: panorama.jpg)\n"
              << "      --top-match-only  Match only the top half (helps moving crowds/cars)\n"
              << "      --max-dim N  Downscale inputs so max(width,height) <= N (0 disables; default: 2000)\n"
              << "      --mode panorama|scans  Use SCANS for translational captures (mosaics)\n"
              << "  -h, --help    Show this help and exit\n"
              << std::endl;
}
