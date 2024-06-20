#include "../header/FileOperations.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void createFolder(const std::string& folderPath)
{
    std::string faceModelsPath = std::move(folderPath + "/FaceModels");
    if (!fs::exists(faceModelsPath)) {
        bool created = fs::create_directory(faceModelsPath);

        if (created) {
            std::cout << "Папка успешно создана.\n";
        }
        else {
            std::cout << "Ошибка при создании папки.\n";
        }
    }
    else {
        std::cout << "Папка уже существует.\n";
    }
    std::cout << '\n';
}

void createXMLFile(const std::string& newFile, const int label)
{
    std::string path = PATH_TO_FACEMODELS_DIR + FILE_NAME + std::to_string(label) + ".xml";

    fs::path directory = fs::path(path).parent_path();
    if (!fs::exists(directory)) {
        std::cerr << "Директория не существует.\n";
        return;
    }

    std::ofstream xmlFile(path);

    if (!xmlFile) {
        std::cerr << "Не удалось создать файл.\n";
        return;
    }
    xmlFile.close();
    std::cout << "XML файл успешно создан по пути " << path << ".\n";
}

bool checkXMLFileExists(const std::string& pathToFile) {
    fs::path filePath = fs::path(pathToFile);

    if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
        return true;
    }

    return false;
}


std::vector<cv::String> getFaceModelsFiles(const std::string& path)
{
    std::vector<cv::String> filesFaceModels;
    for (const auto& entry : fs::directory_iterator(path))
        filesFaceModels.push_back(entry.path().string());
    return filesFaceModels;
}

