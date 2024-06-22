#include "facerec/Preprocess.hpp"

#include <opencv2/imgproc.hpp>

namespace {
const cv::Size kFaceSize(100, 100);
}

cv::Mat toEqualizedGray(const cv::Mat& frame) {
    cv::Mat gray;
    if (frame.channels() == 1) {
        gray = frame.clone();
    } else {
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    }
    cv::equalizeHist(gray, gray);
    return gray;
}

cv::Mat preprocessFace(const cv::Mat& gray, const cv::Rect& face) {
    cv::Mat resized;
    cv::resize(gray(face & cv::Rect(0, 0, gray.cols, gray.rows)), resized, kFaceSize, 0, 0, cv::INTER_AREA);
    return resized;
}
