#pragma once

#include "facerec/FaceModelTrainer.hpp"

class FaceRecognition {
private:
    FaceModelTrainer* faceModel_;
    std::string cascadePath_;
    int cameraIndex_;
    double threshold_;

public:
    FaceRecognition(FaceModelTrainer* modelTrainer, std::string cascadePath, int cameraIndex, double threshold);

    bool recognizeFaces();
};
