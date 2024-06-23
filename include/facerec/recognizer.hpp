#pragma once

#include <opencv2/core.hpp>

#include <vector>

#include "facerec/config.hpp"
#include "facerec/model_store.hpp"

namespace facerec {

struct Recognition {
    bool recognized = false;
    int id = -1;
    double distance = 0.0;  // LBPH distance to the closest person; lower is more similar
};

class Recognizer {
public:
    Recognizer(std::vector<PersonModel> models, double threshold);

    bool empty() const { return models_.empty(); }

    // face must come from preprocessFace().
    Recognition predict(const cv::Mat& face) const;

private:
    std::vector<PersonModel> models_;
    double threshold_;
};

// Draws the face box and "ID n (distance)" / "Unknown" above it.
void drawRecognition(cv::Mat& frame, const cv::Rect& face, const Recognition& result);

// Live recognition from the camera until Esc/q. Returns an ExitCode.
int runRecognition(const Config& config);

}  // namespace facerec
