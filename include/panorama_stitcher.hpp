#pragma once

#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/stitching.hpp>

// Simple wrapper around OpenCV's Stitcher.
class OpenCVStitcher {
public:
    // Configure stitcher; defaults to panoramic mode.
    OpenCVStitcher();

    // Stitch input images into 'output'. Returns true on success; otherwise
    // returns false and sets 'error_message'.
    // 'mode' can be cv::Stitcher::PANORAMA (default) or cv::Stitcher::SCANS for translational captures.
    bool stitch(const std::vector<cv::Mat>& images,
                cv::Mat& output,
                std::string& error_message,
                cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA) const;

    // Optional tuning knobs
    void set_confidence_threshold(float thresh) { confidence_thresh_ = thresh; }
    void set_wave_correction(bool enable) { do_wave_correct_ = enable; }
    // Limit keypoint/search area to top half of the images
    void set_top_match_only(bool enable) { top_match_only_ = enable; }

private:
    float confidence_thresh_ = 0.6f; // default confidence for seam finder/matcher
    bool do_wave_correct_ = true;
    bool top_match_only_ = false;
};

