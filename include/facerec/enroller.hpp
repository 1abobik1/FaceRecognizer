#pragma once

#include <opencv2/core.hpp>

#include <vector>

#include "facerec/camera.hpp"
#include "facerec/config.hpp"
#include "facerec/face_detector.hpp"

namespace facerec {

enum class CaptureStatus { Done, Cancelled, CameraError };

// Collects `count` preprocessed face samples from the camera, at most one per kSampleInterval.
// Only the largest face in the frame is used, so other people do not end up in the model.
CaptureStatus collectSamples(Camera& camera, FaceDetector& detector, int count,
                             std::vector<cv::Mat>& samples);

enum class EnrollMode { Create, Update };

// Create: trains a new model for config.personId (the ID must not exist yet).
// Update: adds new samples to the existing model. Returns an ExitCode.
int runEnroll(const Config& config, EnrollMode mode);

}  // namespace facerec
