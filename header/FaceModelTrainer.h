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

public:
    FaceModelTrainer();

    void captureAndAddFace(int label);
    void trainNewModel(const std::string& modelFileName);
    void loadModels(const std::vector<cv::String>& modelFileNames);
    void addFace(const cv::Mat& face, int label);
    void updateModel(const cv::String& modelFileName);

    std::vector<cv::Ptr<cv::face::LBPHFaceRecognizer>> getModels() const;
    std::vector<cv::Mat> getImages() const;
    std::vector<int> getLabels() const;
};
