#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "cli.hpp"
#include "image_io.hpp"
#include "panorama_stitcher.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    // Parse CLI
    CLIOptions opts = parse_cli(argc, argv);
    if (opts.show_help) {
        print_help(argv[0]);
        return 0;
    }

    try {
        // Resolve and validate paths
        fs::path inputDir = opts.input_dir.empty() ? fs::path("images") : fs::path(opts.input_dir);
        fs::path outputDir = opts.output_dir.empty() ? fs::path("output") : fs::path(opts.output_dir);

        if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
            std::cerr << "Input directory does not exist or is not a directory: " << inputDir << "\n";
            return 2;
        }

        if (!fs::exists(outputDir)) {
            std::error_code ec;
            fs::create_directories(outputDir, ec);
            if (ec) {
                std::cerr << "Failed to create output directory: " << outputDir << " (" << ec.message() << ")\n";
                return 3;
            }
        }

        // Convert any HEIC/HEIF images to JPG first (non-recursive)
        {
            std::string report;
            std::size_t converted = utils::convert_heic_to_jpg_in_dir(inputDir, 95, false, &report);
            if (converted > 0) {
                std::cout << "Converted " << converted << " HEIC image(s) to JPG in '" << inputDir << "'\n";
            }
            if (!report.empty()) {
                std::cout << report; // include per-file results
            }
        }

        // Gather images
        std::vector<fs::path> imagePaths = list_image_files(inputDir);
        if (imagePaths.empty()) {
            std::cerr << "No images found in: " << inputDir << "\n";
            return 4;
        }

        // Load images
        std::vector<cv::Mat> images;
        images.reserve(imagePaths.size());
        for (const auto& p : imagePaths) {
            cv::Mat img = cv::imread(p.string(), cv::IMREAD_COLOR);
            if (img.empty()) {
                std::cerr << "Warning: failed to load image: " << p << "\n";
                continue;
            }
            // Optional downscale to limit memory and speed up
            if (opts.max_dim > 0) {
                int w = img.cols, h = img.rows;
                int m = std::max(w, h);
                if (m > opts.max_dim) {
                    double scale = static_cast<double>(opts.max_dim) / static_cast<double>(m);
                    cv::resize(img, img, cv::Size(), scale, scale, cv::INTER_AREA);
                }
            }
            images.emplace_back(std::move(img));
        }

        if (images.empty()) {
            std::cerr << "Failed to load any images from: " << inputDir << "\n";
            return 5;
        }

        // If only one image provided, save it directly as the panorama
        fs::path outFile = outputDir / (opts.output_filename.empty() ? fs::path("panorama.jpg") : fs::path(opts.output_filename));

        if (images.size() == 1) {
            if (!cv::imwrite(outFile.string(), images.front())) {
                std::cerr << "Failed to save output image to: " << outFile << "\n";
                return 6;
            }
            std::cout << "Only one image provided; copied to output: " << outFile << "\n";
            return 0;
        }

        // Stitch panorama
        cv::Mat pano;
        std::string err;
        OpenCVStitcher stitcher;
        stitcher.set_top_match_only(opts.top_match_only);
        cv::Stitcher::Mode mode = (opts.mode == "scans") ? cv::Stitcher::SCANS : cv::Stitcher::PANORAMA;
        std::vector<cv::Mat> test_images = {images[1], images[2], images[3]}; // For debugging
        if (!stitcher.stitch(test_images, pano, err, mode)) {
            std::cerr << "Stitching failed: " << err << "\n";
            return 7;
        }

        // Trim black bands (top/bottom) from result
        cv::Mat trimmed = utils::trim_black_bands_top_bottom(pano, 10, 0.05, 2);

        // Save result
        if (!cv::imwrite(outFile.string(), trimmed)) {
            std::cerr << "Failed to save panorama to: " << outFile << "\n";
            return 8;
        }

        std::cout << "Panorama saved to: " << outFile << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Unhandled exception: " << ex.what() << "\n";
        return 100;
    }
}
