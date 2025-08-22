#include "panorama_stitcher.hpp"

#include <opencv2/stitching.hpp>
#include <opencv2/stitching/detail/util.hpp>
#include <sstream>

PanoramaStitcher::PanoramaStitcher() = default;

static const char* status_to_cstr(cv::Stitcher::Status s) {
    switch (s) {
        case cv::Stitcher::OK: return "OK";
        case cv::Stitcher::ERR_NEED_MORE_IMGS: return "ERR_NEED_MORE_IMGS";
        case cv::Stitcher::ERR_HOMOGRAPHY_EST_FAIL: return "ERR_HOMOGRAPHY_EST_FAIL";
        case cv::Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL: return "ERR_CAMERA_PARAMS_ADJUST_FAIL";
        default: return "UNKNOWN";
    }
}

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
            oss << "OpenCV Stitcher failed with status: " << status_to_cstr(status);
            error_message = oss.str();
            return false;
        }
        return true;
    } catch (const std::exception& ex) {
        error_message = ex.what();
        return false;
    }
}
