#include "facerec/FaceModelTrainer.hpp"
#include "facerec/Preprocess.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>

namespace {
// Consecutive frames are nearly identical; a pause gives the person time to move a little.
constexpr std::chrono::milliseconds kSampleInterval(200);
}

FaceModelTrainer::FaceModelTrainer(std::string cascadePath, int cameraIndex, int samplesPerPerson)
    : cascadePath_(std::move(cascadePath)), cameraIndex_(cameraIndex), samplesPerPerson_(samplesPerPerson) {}

bool FaceModelTrainer::captureAndAddFace(int label) {
    cv::VideoCapture cap(cameraIndex_);
    if (!cap.isOpened()) {
        std::cerr << "Cannot open the video camera" << std::endl;
        return false;
    }

    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load(cascadePath_)) {
        std::cerr << "Error loading face cascade file: " << cascadePath_ << std::endl;
        return false;
    }

    std::vector<cv::Mat> capturedFaces;
    int count = 0;
    auto lastSample = std::chrono::steady_clock::now() - kSampleInterval;

    while (count < samplesPerPerson_) {
        cv::Mat frame;
        bool bSuccess = cap.read(frame);
        if (!bSuccess) {
            std::cout << "Video camera is disconnected" << std::endl;
            break;
        }

        cv::Mat gray = toEqualizedGray(frame);
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 10, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

        if (!faces.empty()) {
            // Only the largest face: other people in the frame must not get into this person's model.
            const cv::Rect face = *std::max_element(faces.begin(), faces.end(),
                [](const cv::Rect& a, const cv::Rect& b) { return a.area() < b.area(); });

            const auto now = std::chrono::steady_clock::now();
            if (now - lastSample >= kSampleInterval) {
                // taken from gray (a separate buffer), so the ellipse drawn on frame does not leak in
                capturedFaces.push_back(preprocessFace(gray, face));
                lastSample = now;
                count++;
            }
            cv::Point center(face.x + face.width / 2, face.y + face.height / 2);
            cv::ellipse(frame, center, cv::Size(face.width / 2, face.height / 2), 0, 0, 360, cv::Scalar(0, 0, 255), 2);
        }

        const std::string progress = std::to_string(count) + "/" + std::to_string(samplesPerPerson_);
        cv::putText(frame, progress, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Capture Faces", frame);
        if (cv::waitKey(30) >= 0) break;
    }

    cv::destroyWindow("Capture Faces");

    for (const auto& face : capturedFaces) {
        addFace(face, label);
    }
    return true;
}

void FaceModelTrainer::addFace(const cv::Mat& face, int label) {
    images_.push_back(face);
    labels_.push_back(label);
}

bool FaceModelTrainer::updateModel(const cv::String& modelFileName)
{
    if (images_.empty()) {
        std::cerr << "No faces were captured, the model was not changed" << std::endl;
        return false;
    }
    cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
    model->read(modelFileName);  // keep the existing histograms, otherwise update() starts from scratch
    model->update(images_, labels_);
    model->save(modelFileName);
    return true;
}

bool FaceModelTrainer::trainNewModel(const std::string& modelFileName) {
    if (images_.empty()) {
        std::cerr << "No faces were captured, the model was not created" << std::endl;
        return false;
    }
    cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
    model->train(images_, labels_);
    model->save(modelFileName);
    return true;
}

void FaceModelTrainer::loadModels(const std::vector<cv::String>& modelFileNames) {
    for (const auto& modelFileName : modelFileNames) {
        cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
        model->read(modelFileName);
        models_.push_back(model);
    }
}

std::vector<cv::Ptr<cv::face::LBPHFaceRecognizer>> FaceModelTrainer::getModels() const {
    return models_;
}

std::vector<cv::Mat> FaceModelTrainer::getImages() const {
    return images_;
}

std::vector<int> FaceModelTrainer::getLabels() const {
    return labels_;
}
