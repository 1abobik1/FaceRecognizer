#pragma once

#include "FaceModelTrainer.h"

class FaceRecognition {
private:
    FaceModelTrainer* faceModel_;
public:
    FaceRecognition(FaceModelTrainer* modelTrainer);

    void recognizeFaces();
};
