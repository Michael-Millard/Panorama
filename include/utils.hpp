#pragma once

#include <filesystem>
#include <string>
#include <opencv2/core.hpp>

namespace utils {

// Returns true if the path has a HEIC/HEIF extension (case-insensitive).
bool is_heic_file(const std::filesystem::path& p);

// Convert a single .HEIC/.HEIF file to .jpg in the same directory.
// - Writes <basename>.jpg next to the source.
// - On success, removes the original HEIC file.
// - Returns true on success; false on failure and sets error_message (if provided).
// - On success, out_jpg_path (if provided) is set to the resulting file path.
bool convert_heic_to_jpg(const std::filesystem::path& heic_path,
                         std::filesystem::path* out_jpg_path = nullptr,
                         int jpg_quality = 95,
                         std::string* error_message = nullptr);

// Convert all HEIC/HEIF files in a directory (non-recursive by default).
// - Returns the number of files successfully converted.
// - If recursive is true, it will traverse subdirectories.
// - Aggregates a brief report into 'report' if provided.
std::size_t convert_heic_to_jpg_in_dir(const std::filesystem::path& dir,
                                       int jpg_quality = 95,
                                       bool recursive = false,
                                       std::string* report = nullptr);

// Trim nearly-black horizontal bands from the top and bottom of an image.
// - black_threshold: pixel intensity (0-255) below/eq which a pixel is considered black.
// - min_black_row_fraction: if >= this fraction of pixels in a row are black, the row is treated as black.
// - extra_crop: additional pixels to crop inside the detected bounds for safety.
// Returns a cropped copy if trimming was applied; otherwise returns a copy of the input.
cv::Mat trim_black_bands_top_bottom(const cv::Mat& input,
                                    int black_threshold = 20,
                                    double min_black_row_fraction = 0.80,
                                    int extra_crop = 0);

} // namespace utils
