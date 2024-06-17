#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>

#include <libpq/libpq-fs.h>

#include <iostream>
#include <string>
#include <string_view>

#include "header/FaceRecognition.h"

static void setLogLevel() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
}

static const bool logLevelSet = (setLogLevel(), true);

using namespace cv;
using namespace std;

int main() {
    faceRecognition();
}