#pragma once

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <string>

namespace facerec {

inline constexpr int kKeyEsc = 27;

// Webcam plus its preview window; the window is closed when the object is destroyed.
class Camera {
public:
    Camera(int index, std::string windowName);
    ~Camera();

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    bool isOpened() const { return capture_.isOpened(); }
    bool read(cv::Mat& frame) { return capture_.read(frame); }

    // Shows the frame and waits for a key for up to delayMs; returns the key code or -1.
    int show(const cv::Mat& frame, int delayMs);

private:
    cv::VideoCapture capture_;
    std::string windowName_;
    bool windowCreated_ = false;
};

}  // namespace facerec
