#include "image_io.hpp"

#include <algorithm>
#include <string>

using std::filesystem::path;

static bool has_image_ext(const path& p) {
    static const char* exts[] = {".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff"};
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
    for (auto e : exts) {
        if (ext == e) {
            return true;
        }
    }
    return false;
}

std::vector<path> list_image_files(const path& dir) {
    std::vector<path> files;
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return files;
    }
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const path& p = entry.path();
        if (has_image_ext(p)) {
            files.push_back(p);
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}
