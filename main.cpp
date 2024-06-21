#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <string_view>

#include "header/FaceRecognition.h"
#include "header/FaceModelTrainer.h"
#include "header/FileOperations.h"

static void setLogLevel() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
}

static const bool logLevelSet = (setLogLevel(), true);

using namespace std;

int main() {
    setlocale(LC_ALL, "ru");

    createFolder(PATH_TO_PROJECT);

    FaceModelTrainer faceModel;
    FaceRecognition faceRecognition(&faceModel);

    int choice;
    cout << "Нажмите цифру 1. Для распознавания лица\n";
    cout << "Нажмите цифру 2. Добавить модель своего лица\n";
    cout << "Нажмите цифру 3. Чтобы дообучить уже имеющуюся модель лица\n";
    cin >> choice;

    switch (choice) {
    case 1:
        faceModel.loadModels(getFaceModelsFiles(PATH_TO_FACEMODELS_DIR));
        faceRecognition.recognizeFaces();
        break;

    case 2: {
        int label;
        cout << "Введите ваш уникальный ID: ";
        cin >> label;
        if (!(checkXMLFileExists(PATH_TO_FACEMODELS_DIR + FILE_NAME + std::to_string(label) + ".xml"))) {
            createXMLFile(PATH_TO_FACEMODELS_DIR + FILE_NAME + std::to_string(label) + ".xml", label);
            faceModel.captureAndAddFace(label);
            faceModel.trainNewModel(PATH_TO_FACEMODELS_DIR + FILE_NAME + std::to_string(label) + ".xml");
            break;
		}
        else {
            std::cout << "Такой ID уже существует\n";
            break;
        }
    }

    case 3: {
        int label;
        cout << "Введите ваш уникальный ID: ";
        cin >> label;

        if (checkXMLFileExists(PATH_TO_FACEMODELS_DIR + FILE_NAME + std::to_string(label) + ".xml")) {
            faceModel.loadModels(getFaceModelsFiles(PATH_TO_FACEMODELS_DIR));
            faceModel.captureAndAddFace(label);
            faceModel.updateModel(PATH_TO_FACEMODELS_DIR + FILE_NAME + std::to_string(label) + ".xml");
            break;
        }

        else {
            std::cout << "ID " << label << " не существует" << '\n';
            break;
        }
    }

    default:
        return 0;
    }
    return 0;
}