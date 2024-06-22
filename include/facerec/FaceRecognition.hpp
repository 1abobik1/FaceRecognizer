#pragma once

#include "facerec/FaceModelTrainer.hpp"

class FaceRecognition {
private:
    FaceModelTrainer* faceModel_;
public:
    FaceRecognition(FaceModelTrainer* modelTrainer);

    void recognizeFaces();
};
