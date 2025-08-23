#include "panorama_stitcher.hpp"

#include <opencv2/stitching.hpp>
#include <opencv2/stitching/detail/util.hpp>
#include <opencv2/stitching/detail/blenders.hpp>
#include <opencv2/stitching/detail/seam_finders.hpp>
#include <opencv2/stitching/detail/exposure_compensate.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
#include <cmath>

OpenCVStitcher::OpenCVStitcher() = default;

static const char* status_to_cstr(cv::Stitcher::Status s) {
    switch (s) {
        case cv::Stitcher::OK: return "OK";
        case cv::Stitcher::ERR_NEED_MORE_IMGS: return "ERR_NEED_MORE_IMGS";
        case cv::Stitcher::ERR_HOMOGRAPHY_EST_FAIL: return "ERR_HOMOGRAPHY_EST_FAIL";
        case cv::Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL: return "ERR_CAMERA_PARAMS_ADJUST_FAIL";
        default: return "UNKNOWN";
    }
}

bool OpenCVStitcher::stitch(const std::vector<cv::Mat>& images, cv::Mat& output, std::string& error_message, cv::Stitcher::Mode mode) const {
    if (images.size() < 1) {
        error_message = "No images provided";
        return false;
    }

    try {
        // Use OpenCV's high-level Stitcher API in the requested mode.
        auto stitcher = cv::Stitcher::create(mode);

        // Configure optional parameters
        stitcher->setPanoConfidenceThresh(confidence_thresh_);
        stitcher->setWaveCorrection(do_wave_correct_);
        if (mode == cv::Stitcher::SCANS) {
            // Translational/mosaic: disable wave correction; use default warper
            stitcher->setWaveCorrection(false);
        }

        // Create masks per-image (match each image size)
        std::vector<cv::Mat> masks;
        masks.reserve(images.size());
        for (const auto& im : images) {
            masks.emplace_back(im.rows, im.cols, CV_8UC1, cv::Scalar(255));
        }
        if (top_match_only_) {
            for (size_t i = 0; i < images.size(); ++i) {
                // Keep top half (zero bottom half)
                masks[i](cv::Rect(0, images[i].rows / 2, images[i].cols, images[i].rows / 2)) = 0;
            }
        }

        // Let stitcher automatically choose features and matchers (OpenCV may default to ORB if SIFT not available)
        cv::Stitcher::Status status = stitcher->stitch(images, masks, output);
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