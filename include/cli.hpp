#pragma once

#include <string>

struct CLIOptions {
    std::string input_dir;
    std::string output_dir;
    std::string output_filename; // default: panorama.jpg
    bool show_help{false};
};

// Parse command line arguments. Supports:
//   -i, --input <dir>
//   -o, --output <dir>
//   -f, --file <filename>
//   -h, --help
CLIOptions parse_cli(int argc, char** argv);

void print_help(const char* prog);
