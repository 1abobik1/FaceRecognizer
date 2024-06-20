#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>

#include <libpq/libpq-fs.h>

#include <iostream>
#include <string>
#include <string_view>

#include "header/FaceRecognition.h"
#include "header/FaceModelTrainer.h"

static void setLogLevel() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
}

static const bool logLevelSet = (setLogLevel(), true);

using namespace std;

int main() {
    setlocale(LC_ALL, "ru");
    FaceModelTrainer faceModel;
    FaceRecognition faceRec(&faceModel);
    int choice;
    cout << "Нажмите цифру 1. Для распознавания лица\n";
    cout << "Нажмите цифру 2, если вас не распознает камера\n";
    cin >> choice;

    switch (choice) {
    case 1:
        faceRec.faceRecognition();
        break;
    case 2: {
        string name, surname;
        cout << "Введите ваше имя: ";
        cin >> name;
        cout << "Введите вашу фамилию: ";
        cin >> surname;
        faceModel.captureAndAddFace(name, surname);
        faceModel.modelTraining("C:/Users/dima1/source/repos/Facerecognizer/ResourcesCV/face_model.xml");
        break;
    }
    default:
        return 0;
    }
    return 0;
}
