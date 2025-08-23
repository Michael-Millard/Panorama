#include "utils.hpp"

#include <algorithm>
#include <cctype>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>

namespace utils {

static std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { 
        return std::tolower(c); 
    });
    return s;
}

bool is_heic_file(const std::filesystem::path& p) {
    std::string ext = to_lower(p.extension().string());
    return ext == ".heic" || ext == ".heif";
}

bool convert_heic_to_jpg(const std::filesystem::path& heic_path,
                         std::filesystem::path* out_jpg_path,
                         int jpg_quality,
                         std::string* error_message) {
    if (!std::filesystem::exists(heic_path) || !std::filesystem::is_regular_file(heic_path)) {
        if (error_message) {
            *error_message = "Input file does not exist or is not a regular file";
        }
        return false;
    }
    if (!is_heic_file(heic_path)) {
        if (error_message) {
            *error_message = "Input file is not .heic/.heif";
        }
        return false;
    }

    // Read using OpenCV (requires OpenCV built with HEIF/HEIC support via libheif).
    cv::Mat img = cv::imread(heic_path.string(), cv::IMREAD_COLOR);
    if (img.empty()) {
        if (error_message) {
            *error_message = "Failed to read HEIC image (check OpenCV HEIF support)";
        }
        return false;
    }

    std::filesystem::path jpg_path = heic_path;
    jpg_path.replace_extension(".jpg");

    std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, std::clamp(jpg_quality, 1, 100) };
    if (!cv::imwrite(jpg_path.string(), img, params)) {
        if (error_message) {
            *error_message = "Failed to write JPEG file";
        }
        return false;
    }

    // Remove original on success
    std::error_code ec;
    std::filesystem::remove(heic_path, ec);
    if (ec) {
        // JPEG was written but original couldn't be removed; treat as partial success
        if (error_message) {
            *error_message = std::string("Converted, but failed to remove original: ") + ec.message();
        }
        if (out_jpg_path) {
            *out_jpg_path = jpg_path;
        }
        return true;
    }

    if (out_jpg_path) {
        *out_jpg_path = jpg_path;
    }
    return true;
}

std::size_t convert_heic_to_jpg_in_dir(const std::filesystem::path& dir,
                                       int jpg_quality,
                                       bool recursive,
                                       std::string* report) {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        if (report) {
            *report = "Directory does not exist or is not a directory";
        }
        return 0;
    }
    std::size_t converted = 0;
    std::ostringstream oss;
    auto handle_file = [&](const std::filesystem::path& p){
        if (!is_heic_file(p)) {
            return;
        }
        std::string err;
        std::filesystem::path outp;
        if (convert_heic_to_jpg(p, &outp, jpg_quality, &err)) {
            ++converted;
            oss << "Converted: " << p.filename().string() << " -> " << outp.filename().string() << "\n";
        } else {
            oss << "Failed: " << p.filename().string() << " (" << err << ")\n";
        }
    };

    if (recursive) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                handle_file(entry.path());
            }
        }
    } else {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                handle_file(entry.path());
            }
        }
    }

    if (report) {
        *report = oss.str();
    }
    return converted;
}

cv::Mat trim_black_bands_top_bottom(const cv::Mat& input,
                                    int black_threshold,
                                    double min_black_row_fraction,
                                    int extra_crop) {
    if (input.empty()) {
        return input.clone();
    }
    CV_Assert(input.channels() == 1 || input.channels() == 3 || input.channels() == 4);

    // Convert to grayscale to measure darkness per row
    cv::Mat gray;
    if (input.channels() == 3) {
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    } else if (input.channels() == 4) {
        cv::cvtColor(input, gray, cv::COLOR_BGRA2GRAY);
    } else {
        gray = input;
    }

    const int rows = gray.rows;
    const int cols = gray.cols;
    const int max_black = std::clamp(black_threshold, 0, 255);
    const int row_black_limit = static_cast<int>(std::clamp(min_black_row_fraction, 0.0, 1.0) * cols);

    auto row_is_black = [&](int r) -> bool {
        const uchar* ptr = gray.ptr<uchar>(r);
        int cnt = 0;
        for (int c = 0; c < cols; ++c) {
            if (ptr[c] <= max_black) {
                ++cnt;
                if (cnt >= row_black_limit) {
                    return true; // enough black to consider this row black
                }
            }
        }
        return false;
    };

    int top = 0;
    while (top < rows && row_is_black(top)) {
        ++top;
    }

    int bottom = rows - 1;
    while (bottom >= 0 && row_is_black(bottom)) {
        --bottom;
    }

    if (top == 0 && bottom == rows - 1) {
        // No black bands detected
        return input.clone();
    }

    // Apply extra safety crop inside bounds
    top = std::min(std::max(0, top + extra_crop), rows - 1);
    bottom = std::max(0, std::min(rows - 1, bottom - extra_crop));

    if (bottom <= top) {
        // Cropped out everything; return original to be safe
        return input.clone();
    }

    cv::Rect roi(0, top, cols, bottom - top + 1);
    return input(roi).clone();
}

} // namespace utils
