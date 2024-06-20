#include "../header/FaceRecognition.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace cv;
using namespace cv::face;
using namespace std;

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

        Mat frameGray;
        cvtColor(frame, frameGray, COLOR_BGR2GRAY);
        equalizeHist(frameGray, frameGray);

        vector<Rect> faces;
        faceCascade.detectMultiScale(frameGray, faces, 1.1, 10, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        for (size_t i = 0; i < faces.size(); i++) {
            Mat faceROI = frameGray(faces[i]);

            bool recognized = false;
            for (const auto& model : models) {
                int label;
                double confidence;
                model->predict(faceROI, label, confidence);

                if (confidence < 50) {
                    cout << "Recognized ID: " << label << " with confidence: " << confidence << '\n';
                    recognized = true;
                    break;
                }
            }

            if (!recognized) {
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
