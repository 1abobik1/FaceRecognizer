#pragma once

#include "FaceModelTrainer.h"

class FaceRecognition {
private:
	FaceModelTrainer* faceModel_;
public:
	FaceRecognition(FaceModelTrainer* faceModel) : faceModel_(faceModel) {}

	void faceRecognition();
};
