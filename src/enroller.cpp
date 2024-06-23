#include "facerec/enroller.hpp"

#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

#include "facerec/model_store.hpp"
#include "facerec/preprocess.hpp"

namespace facerec {

namespace {
// Consecutive frames are nearly identical; a pause gives the person time to move a little.
constexpr std::chrono::milliseconds kSampleInterval(200);
constexpr int kFrameDelayMs = 30;
const cv::Scalar kFaceColor(0, 0, 255);
const cv::Scalar kTextColor(0, 255, 0);
}  // namespace

CaptureStatus collectSamples(Camera& camera, FaceDetector& detector, int count,
                             std::vector<cv::Mat>& samples) {
    auto lastSample = std::chrono::steady_clock::now() - kSampleInterval;
    cv::Mat frame;

    while (static_cast<int>(samples.size()) < count) {
        if (!camera.read(frame)) {
            std::cerr << "Камера отключилась\n";
            return CaptureStatus::CameraError;
        }

        // Samples are cut from gray (a separate buffer), so what is drawn on frame does not leak into them.
        const cv::Mat gray = toEqualizedGray(frame);
        const std::vector<cv::Rect> faces = detector.detect(gray);
        if (!faces.empty()) {
            const cv::Rect face =
                *std::max_element(faces.begin(), faces.end(),
                                  [](const cv::Rect& a, const cv::Rect& b) { return a.area() < b.area(); });

            const auto now = std::chrono::steady_clock::now();
            if (now - lastSample >= kSampleInterval) {
                samples.push_back(preprocessFace(gray, face));
                lastSample = now;
            }
            cv::rectangle(frame, face, kFaceColor, 2);
        }

        const std::string progress = std::to_string(samples.size()) + "/" + std::to_string(count);
        cv::putText(frame, progress, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, kTextColor, 2);

        if (camera.show(frame, kFrameDelayMs) == kKeyEsc) {
            return CaptureStatus::Cancelled;
        }
    }
    return CaptureStatus::Done;
}

int runEnroll(const Config& config, EnrollMode mode) {
    const ModelStore store(config.modelsDir);
    const int id = config.personId;
    const bool exists = store.exists(id);

    if (mode == EnrollMode::Create && exists) {
        std::cerr << "ID " << id << " уже существует. Дообучить: facerec update --id " << id << '\n';
        return kExitUsage;
    }
    if (mode == EnrollMode::Update && !exists) {
        std::cerr << "ID " << id << " не существует. Добавить: facerec enroll --id " << id << '\n';
        return kExitUsage;
    }

    // Load before opening the camera, so a broken file is reported without making the user pose for nothing.
    LbphModel model = mode == EnrollMode::Update ? store.load(id) : cv::face::LBPHFaceRecognizer::create();
    if (!model) {
        return kExitRuntime;
    }

    FaceDetector detector;
    if (!loadDetector(config, detector)) {
        return kExitRuntime;
    }

    std::vector<cv::Mat> samples;
    {
        Camera camera(config.cameraIndex, "Capture Faces");
        if (!camera.isOpened()) {
            std::cerr << "Не удалось открыть камеру " << config.cameraIndex << '\n';
            return kExitRuntime;
        }
        std::cout << "Смотрите в камеру и слегка поворачивайте голову. Esc - отмена.\n";

        switch (collectSamples(camera, detector, config.samplesPerEnroll, samples)) {
            case CaptureStatus::Done:
                break;
            case CaptureStatus::Cancelled:
                std::cout << "Съёмка отменена, модель не изменена\n";
                return kExitOk;
            case CaptureStatus::CameraError:
                return kExitRuntime;
        }
    }

    const std::vector<int> labels(samples.size(), id);
    if (mode == EnrollMode::Create) {
        model->train(samples, labels);
    } else {
        model->update(samples, labels);  // keeps the histograms already in the model
    }

    if (!store.save(id, model)) {
        return kExitRuntime;
    }
    std::cout << (mode == EnrollMode::Create ? "Модель сохранена: " : "Модель обновлена: ")
              << store.pathFor(id).string() << '\n';
    return kExitOk;
}

}  // namespace facerec
