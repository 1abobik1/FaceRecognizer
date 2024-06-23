#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <vector>
#include <string>

class FaceModelTrainer {
private:
    std::vector<cv::Ptr<cv::face::LBPHFaceRecognizer>> models_;
    std::vector<cv::Mat> images_;
    std::vector<int> labels_;
    std::string cascadePath_;
    int cameraIndex_;
    int samplesPerPerson_;

public:
    FaceModelTrainer(std::string cascadePath, int cameraIndex, int samplesPerPerson);

    bool captureAndAddFace(int label);
    bool trainNewModel(const std::string& modelFileName);
    void loadModels(const std::vector<cv::String>& modelFileNames);
    void addFace(const cv::Mat& face, int label);
    bool updateModel(const cv::String& modelFileName);

    std::vector<cv::Ptr<cv::face::LBPHFaceRecognizer>> getModels() const;
    std::vector<cv::Mat> getImages() const;
    std::vector<int> getLabels() const;
};
