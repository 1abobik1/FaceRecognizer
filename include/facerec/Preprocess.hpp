#pragma once

#include <opencv2/core.hpp>

// Frame -> grayscale with equalized histogram; used for detection.
cv::Mat toEqualizedGray(const cv::Mat& frame);

// Crops the face from an equalized gray frame and resizes it to a fixed size.
// The same function is used for training and recognition, so LBPH compares like with like.
cv::Mat preprocessFace(const cv::Mat& gray, const cv::Rect& face);
