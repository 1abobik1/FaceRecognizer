#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <vector>

class FaceModelTrainer {
private:
    cv::Ptr<cv::face::LBPHFaceRecognizer> model_ = cv::face::LBPHFaceRecognizer::create();
    std::vector<cv::Mat> images_;
    std::vector<int> labels_;

public:
    void modelTraining(const cv::String& modelFileName);
    void addFace(const cv::Mat& face, int label);
    void captureAndAddFace(const std::string& name, const std::string& surname);

    cv::Ptr<cv::face::LBPHFaceRecognizer> getModel() const;
    std::vector<cv::Mat> getImages() const;
    std::vector<int> getLabels() const;
};
