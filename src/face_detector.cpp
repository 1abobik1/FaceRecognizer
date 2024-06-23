#include "facerec/face_detector.hpp"

#include <iostream>

namespace facerec {

namespace {
constexpr double kScaleFactor = 1.1;
// High value: fewer false detections at the cost of missing some faces.
constexpr int kMinNeighbors = 10;
const cv::Size kMinFaceSize(30, 30);
}  // namespace

bool FaceDetector::load(const std::filesystem::path& cascadePath) {
    return cascade_.load(cascadePath.string());
}

std::vector<cv::Rect> FaceDetector::detect(const cv::Mat& gray) {
    std::vector<cv::Rect> faces;
    cascade_.detectMultiScale(gray, faces, kScaleFactor, kMinNeighbors, cv::CASCADE_SCALE_IMAGE,
                              kMinFaceSize);
    return faces;
}

bool loadDetector(const Config& config, FaceDetector& detector) {
    const auto cascade = resolveCascadePath(config);
    if (!cascade) {
        std::cerr << "Не найден haarcascade_frontalface_default.xml. Укажите путь через --cascade "
                     "или переменную окружения FACEREC_CASCADE\n";
        return false;
    }
    if (!detector.load(*cascade)) {
        std::cerr << "Не удалось загрузить каскад Хаара: " << cascade->string() << '\n';
        return false;
    }
    return true;
}

}  // namespace facerec
