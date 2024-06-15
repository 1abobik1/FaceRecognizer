#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

#include "./header/VideoCapture.h"
#include "./header/FaceDetection.h"

static void setLogLevel() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
}

static const bool logLevelSet = (setLogLevel(), true);

int main()
{
    //playVideo();
    faceSearch();
    return 0;
}