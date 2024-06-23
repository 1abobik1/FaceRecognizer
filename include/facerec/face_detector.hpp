#pragma once

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

#include <filesystem>
#include <vector>

#include "facerec/config.hpp"

namespace facerec {

// Haar cascade face detector.
class FaceDetector {
public:
    bool load(const std::filesystem::path& cascadePath);

    // Expects an equalized grayscale frame (see toEqualizedGray()).
    std::vector<cv::Rect> detect(const cv::Mat& gray);

private:
    cv::CascadeClassifier cascade_;
};

// Finds the cascade via resolveCascadePath() and loads it; prints the reason on failure.
bool loadDetector(const Config& config, FaceDetector& detector);

}  // namespace facerec
