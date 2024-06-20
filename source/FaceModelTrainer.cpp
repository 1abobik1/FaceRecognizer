#include "../header/FaceModelTrainer.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

FaceModelTrainer::FaceModelTrainer() {}

void FaceModelTrainer::captureAndAddFace(int label) {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Cannot open the video camera" << std::endl;
        return;
    }

    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load("C:/Users/dima1/source/repos/Facerecognizer/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cout << "Error loading face cascade file" << std::endl;
        return;
    }

    std::vector<cv::Mat> capturedFaces;
    int count = 0;

    while (count < 20) {
        cv::Mat frame;
        bool bSuccess = cap.read(frame);
        if (!bSuccess) {
            std::cout << "Video camera is disconnected" << std::endl;
            break;
        }

        cv::Mat frameGray;
        cv::cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(frameGray, frameGray);

        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(frameGray, faces, 1.1, 10, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

        for (size_t i = 0; i < faces.size(); i++) {
            cv::Mat faceROI = frameGray(faces[i]);
            capturedFaces.push_back(faceROI);
            cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
            cv::ellipse(frame, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cv::Scalar(0, 0, 255), 2);
            count++;
            if (count >= 20) break;
        }

        cv::imshow("Capture Faces", frame);
        if (cv::waitKey(30) >= 0) break;
    }

    cv::destroyWindow("Capture Faces");

    for (const auto& face : capturedFaces) {
        addFace(face, label);
    }
}

void FaceModelTrainer::addFace(const cv::Mat& face, int label) {
    cv::Mat gray;
    if (face.channels() == 3) {
        cv::cvtColor(face, gray, cv::COLOR_BGR2GRAY);
    }
    else {
        gray = face; 
    }
    cv::equalizeHist(gray, gray);
    images_.push_back(gray);
    labels_.push_back(label);
}

void FaceModelTrainer::updateModel(const cv::String& modelFileName,const int label)
{
    models_[label]->update(images_, labels_);
    models_[label]->save(modelFileName);
}

void FaceModelTrainer::trainNewModel(const std::string& modelFileName) {
    cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
    model->train(images_, labels_);
    model->save(modelFileName);

    models_.push_back(model);
}

void FaceModelTrainer::loadModels(const std::vector<cv::String>& modelFileNames) {
    for (const auto& modelFileName : modelFileNames) {
        cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
        model->read(modelFileName);
        models_.push_back(model);
    }
}

std::vector<cv::Ptr<cv::face::LBPHFaceRecognizer>> FaceModelTrainer::getModels() const {
    return models_;
}

std::vector<cv::Mat> FaceModelTrainer::getImages() const {
    return images_;
}

std::vector<int> FaceModelTrainer::getLabels() const {
    return labels_;
}
