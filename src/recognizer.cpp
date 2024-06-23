#include "facerec/recognizer.hpp"

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <utility>

#include "facerec/camera.hpp"
#include "facerec/face_detector.hpp"
#include "facerec/preprocess.hpp"

namespace facerec {

namespace {
constexpr int kFrameDelayMs = 10;
const cv::Scalar kKnownColor(0, 200, 0);
const cv::Scalar kUnknownColor(0, 0, 255);
}  // namespace

Recognizer::Recognizer(std::vector<PersonModel> models, double threshold)
    : models_(std::move(models)), threshold_(threshold) {}

Recognition Recognizer::predict(const cv::Mat& face) const {
    // Pick the closest person, not the first model that passes the threshold.
    Recognition best;
    best.distance = std::numeric_limits<double>::max();
    for (const PersonModel& person : models_) {
        int label = -1;
        double distance = 0.0;
        person.model->predict(face, label, distance);
        if (distance < best.distance) {
            best.distance = distance;
            best.id = person.id;
        }
    }
    best.recognized = best.id != -1 && best.distance < threshold_;
    return best;
}

void drawRecognition(cv::Mat& frame, const cv::Rect& face, const Recognition& result) {
    const cv::Scalar& color = result.recognized ? kKnownColor : kUnknownColor;
    const std::string caption =
        result.recognized ? cv::format("ID %d (%.1f)", result.id, result.distance) : std::string("Unknown");

    cv::rectangle(frame, face, color, 2);
    cv::putText(frame, caption, cv::Point(face.x, std::max(face.y - 8, 15)), cv::FONT_HERSHEY_SIMPLEX, 0.7, color,
                2);
}

int runRecognition(const Config& config) {
    const ModelStore store(config.modelsDir);
    const Recognizer recognizer(store.loadAll(), config.threshold);
    if (recognizer.empty()) {
        std::cerr << "Нет ни одной модели в " << store.dir().string()
                  << ". Сначала добавьте лицо: facerec enroll --id N\n";
        return kExitRuntime;
    }

    FaceDetector detector;
    if (!loadDetector(config, detector)) {
        return kExitRuntime;
    }

    Camera camera(config.cameraIndex, "Face Recognition");
    if (!camera.isOpened()) {
        std::cerr << "Не удалось открыть камеру " << config.cameraIndex << '\n';
        return kExitRuntime;
    }
    std::cout << "Распознавание запущено. Esc или q - выход.\n";

    cv::Mat frame;
    while (camera.read(frame)) {
        const cv::Mat gray = toEqualizedGray(frame);
        for (const cv::Rect& face : detector.detect(gray)) {
            drawRecognition(frame, face, recognizer.predict(preprocessFace(gray, face)));
        }

        const int key = camera.show(frame, kFrameDelayMs);
        if (key == kKeyEsc || key == 'q') {
            return kExitOk;
        }
    }
    std::cerr << "Камера отключилась\n";
    return kExitRuntime;
}

}  // namespace facerec
