#pragma once

#include <string>

struct CLIOptions {
    std::string input_dir;
    std::string output_dir;
    std::string output_filename; // default: panorama.jpg
    bool show_help{false};
    // Advanced options
    bool top_match_only{false};   // Match only top half when estimating/matching
    int max_dim{1000};            // Downscale inputs so max(width,height) <= max_dim; 0 to disable
    std::string mode{"panorama"}; // "panorama" or "scans" for translational captures
};

// Parse command line arguments. Supports:
//   -i, --input <dir>
//   -o, --output <dir>
//   -f, --file <filename>
//   -h, --help
//   --top-match-only
//   --max-dim <int>
//   --mode panorama|scans
CLIOptions parse_cli(int argc, char** argv);

void print_help(const char* prog);
