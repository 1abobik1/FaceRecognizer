#include "facerec/FaceRecognition.hpp"
#include "facerec/Preprocess.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <limits>

using namespace cv;
using namespace cv::face;
using namespace std;

namespace {
constexpr double kRecognitionThreshold = 50.0;
}

FaceRecognition::FaceRecognition(FaceModelTrainer* modelTrainer) : faceModel_(modelTrainer) {}


void FaceRecognition::recognizeFaces() {
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Cannot open the video camera\n";
        return;
    }

    CascadeClassifier faceCascade;
    if (!faceCascade.load("C:/Users/dima1/source/repos/Facerecognizer/haarcascades/haarcascade_frontalface_default.xml")) {
        cout << "Error loading face cascade file" << endl;
        return;
    }
    auto models = faceModel_->getModels();
    string window_name = "Face Recognition";
    namedWindow(window_name, WINDOW_NORMAL);

    while (true) {
        Mat frame;
        bool bSuccess = cap.read(frame);
        if (!bSuccess) {
            cout << "Video camera is disconnected\n";
            break;
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

            if (bestLabel != -1 && bestDistance < kRecognitionThreshold) {
                cout << "Recognized ID: " << bestLabel << " with distance: " << bestDistance << '\n';
            } else {
                cout << "Face not recognized.\n";
            }

            Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
            ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(0, 0, 255), 2);
        }

        imshow(window_name, frame);

        if (waitKey(10) == 27) {
            cout << "ESC key is pressed by user. Exiting the program\n";
            break;
        }
    }
}
