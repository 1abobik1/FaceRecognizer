#include "facerec/FileOperations.hpp"

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
    if (!fs::is_directory(path)) {
        return filesFaceModels;
    }
    for (const auto& entry : fs::directory_iterator(path)) {
        const std::string name = entry.path().filename().string();
        const bool isModel = entry.is_regular_file() && name.starts_with(FILE_NAME) && name.ends_with(".xml");
        if (isModel) {
            filesFaceModels.push_back(entry.path().string());
        }
    }
    return filesFaceModels;
}

