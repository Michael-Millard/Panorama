#pragma once

#include <vector>
#include <filesystem>
#include <opencv2/core.hpp>

// List image files in the directory, sorted by filename ascending.
// Accepts common image extensions.
std::vector<std::filesystem::path> list_image_files(const std::filesystem::path& dir);
