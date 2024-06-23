#pragma once

#include <opencv2/core.hpp>

namespace facerec {

// Size every face is scaled to before training and recognition.
inline const cv::Size kFaceSize(100, 100);

// Frame -> grayscale with an equalized histogram; used for detection.
cv::Mat toEqualizedGray(const cv::Mat& frame);

// Crops the face from an equalized gray frame and scales it to kFaceSize.
// The same function is used for training and recognition, so LBPH compares like with like.
cv::Mat preprocessFace(const cv::Mat& gray, const cv::Rect& face);

}  // namespace facerec
