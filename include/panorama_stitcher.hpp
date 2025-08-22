#pragma once

#include <string>
#include <vector>
#include <opencv2/core.hpp>

// Simple wrapper around OpenCV's Stitcher.
class PanoramaStitcher {
public:
    // Configure stitcher; defaults to panoramic mode.
    PanoramaStitcher();

    // Stitch input images into 'output'. Returns true on success; otherwise
    // returns false and sets 'error_message'.
    bool stitch(const std::vector<cv::Mat>& images, cv::Mat& output, std::string& error_message) const;

    // Optional tuning knobs
    void set_confidence_threshold(float thresh) { confidence_thresh_ = thresh; }
    void set_wave_correction(bool enable) { do_wave_correct_ = enable; }

private:
    float confidence_thresh_ = 0.6f; // default confidence for seam finder/matcher
    bool do_wave_correct_ = true;
};
