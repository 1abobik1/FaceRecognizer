#include "facerec/FaceRecognition.hpp"
#include "facerec/Preprocess.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <algorithm>
#include <iostream>
#include <limits>

using namespace cv;
using namespace cv::face;
using namespace std;

FaceRecognition::FaceRecognition(FaceModelTrainer* modelTrainer, std::string cascadePath, int cameraIndex,
                                 double threshold)
    : faceModel_(modelTrainer), cascadePath_(std::move(cascadePath)), cameraIndex_(cameraIndex), threshold_(threshold) {}

bool FaceRecognition::recognizeFaces() {
    VideoCapture cap(cameraIndex_);
    if (!cap.isOpened()) {
        cerr << "Cannot open the video camera\n";
        return false;
    }

    CascadeClassifier faceCascade;
    if (!faceCascade.load(cascadePath_)) {
        cerr << "Error loading face cascade file: " << cascadePath_ << endl;
        return false;
    }
    auto models = faceModel_->getModels();
    string window_name = "Face Recognition";
    namedWindow(window_name, WINDOW_NORMAL);

    while (true) {
        Mat frame;
        bool bSuccess = cap.read(frame);
        if (!bSuccess) {
            cerr << "Video camera is disconnected\n";
            return false;
        }

        Mat frameGray = toEqualizedGray(frame);

        vector<Rect> faces;
        faceCascade.detectMultiScale(frameGray, faces, 1.1, 10, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        for (size_t i = 0; i < faces.size(); i++) {
            Mat faceROI = preprocessFace(frameGray, faces[i]);

            // LBPH returns a distance: the lower, the more similar. Pick the closest person,
            // not the first model that passes the threshold.
            int bestLabel = -1;
            double bestDistance = std::numeric_limits<double>::max();
            for (const auto& model : models) {
                int label = -1;
                double distance = 0.0;
                model->predict(faceROI, label, distance);
                if (distance < bestDistance) {
                    bestDistance = distance;
                    bestLabel = label;
                }
            }

            const bool recognized = bestLabel != -1 && bestDistance < threshold_;
            const Scalar color = recognized ? Scalar(0, 200, 0) : Scalar(0, 0, 255);
            string caption = "Unknown";
            if (recognized) {
                caption = cv::format("ID %d (%.1f)", bestLabel, bestDistance);
            }

            rectangle(frame, faces[i], color, 2);
            putText(frame, caption, Point(faces[i].x, std::max(faces[i].y - 8, 15)), FONT_HERSHEY_SIMPLEX, 0.7,
                    color, 2);
        }

        imshow(window_name, frame);

        if (waitKey(10) == 27) {
            return true;
        }
    }
}
