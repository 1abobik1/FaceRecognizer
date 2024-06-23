#include "facerec/preprocess.hpp"

#include <opencv2/imgproc.hpp>

namespace facerec {

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
    const cv::Rect inside = face & cv::Rect(0, 0, gray.cols, gray.rows);
    cv::Mat resized;
    cv::resize(gray(inside), resized, kFaceSize, 0, 0, cv::INTER_AREA);
    return resized;
}

}  // namespace facerec
