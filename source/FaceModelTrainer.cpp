#include "../header/FaceModelTrainer.h"
#include "../header/DataBase.h"

void FaceModelTrainer::modelTraining(const cv::String& modelFileName) {
    model_->train(images_, labels_);
    model_->save(modelFileName);
}

void FaceModelTrainer::addFace(const cv::Mat& face, int label) {
    cv::Mat gray;
    if (face.channels() == 3) {
        cv::cvtColor(face, gray, cv::COLOR_BGR2GRAY);
    }
    else {
        gray = face; // Если изображение уже в оттенках серого, просто копируем его
    }
    cv::equalizeHist(gray, gray);
    images_.push_back(gray);
    labels_.push_back(label);
}


void FaceModelTrainer::captureAndAddFace(const std::string& name, const std::string& surname) {

    Database::getDatabase()->getUserTable()->addUserToDB(name, surname);
    int label = Database::getDatabase()->getUserTable()->getUserID(name, surname);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "Cannot open the video camera" << std::endl;
        return;
    }

    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load("C:/Users/dima1/source/repos/Facerecognizer/ResourcesCV/haarcascade_frontalface_default.xml")) {
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

cv::Ptr<cv::face::LBPHFaceRecognizer> FaceModelTrainer::getModel() const 
{
    return model_;
}

std::vector<cv::Mat> FaceModelTrainer::getImages() const
{
    return images_;
}

std::vector<int> FaceModelTrainer::getLabels() const
{
    return labels_;
}
