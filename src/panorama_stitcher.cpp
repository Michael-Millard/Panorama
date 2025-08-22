#include "panorama_stitcher.hpp"

#include <opencv2/stitching.hpp>
#include <opencv2/stitching/detail/util.hpp>
#include <sstream>

PanoramaStitcher::PanoramaStitcher() = default;

bool PanoramaStitcher::stitch(const std::vector<cv::Mat>& images, cv::Mat& output, std::string& error_message) const {
    if (images.size() < 1) {
        error_message = "No images provided";
        return false;
    }

    try {
        // Use OpenCV's high-level Stitcher API in PANORAMA mode.
        auto stitcher = cv::Stitcher::create(cv::Stitcher::PANORAMA);

        // Configure optional parameters
        stitcher->setPanoConfidenceThresh(confidence_thresh_);
        stitcher->setWaveCorrection(do_wave_correct_);

        // Let stitcher automatically choose features and matchers (OpenCV may default to ORB if SIFT not available)
        cv::Stitcher::Status status = stitcher->stitch(images, output);
        if (status != cv::Stitcher::OK) {
            std::ostringstream oss;
            oss << "OpenCV Stitcher failed with status: " << static_cast<int>(status);
            error_message = oss.str();
            return false;
        }
        return true;
    } catch (const std::exception& ex) {
        error_message = ex.what();
        return false;
    }
}
