#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>

#include <libpq/libpq-fs.h>

#include <iostream>
#include <string>
#include <string_view>

#include "./header/VideoCapture.h"
#include "./header/FaceDetection.h"

static void setLogLevel() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
}

static const bool logLevelSet = (setLogLevel(), true);

// path to the DB
constexpr std::string_view CONNECTION_DB = "host=localhost dbname=face_DB user=postgres password=Zopa_kek12";


using namespace cv;
using namespace std;

int main() {
    
}