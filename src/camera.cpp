#include "facerec/camera.hpp"

#include <opencv2/highgui.hpp>

#include <utility>

namespace facerec {

Camera::Camera(int index, std::string windowName) : capture_(index), windowName_(std::move(windowName)) {}

Camera::~Camera() {
    if (windowCreated_) {
        cv::destroyWindow(windowName_);
        cv::waitKey(1);  // lets the GUI backend actually close the window
    }
}

int Camera::show(const cv::Mat& frame, int delayMs) {
    if (!windowCreated_) {
        cv::namedWindow(windowName_, cv::WINDOW_NORMAL);
        windowCreated_ = true;
    }
    cv::imshow(windowName_, frame);
    return cv::waitKey(delayMs);
}

}  // namespace facerec
