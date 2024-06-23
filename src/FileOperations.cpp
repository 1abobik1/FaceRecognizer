#include "facerec/FileOperations.hpp"

#include <iostream>
#include <system_error>

namespace fs = std::filesystem;

bool ensureDirectory(const fs::path& dir) {
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) {
        std::cerr << "Не удалось создать каталог " << dir.string() << ": " << ec.message() << '\n';
        return false;
    }
    return true;
}

fs::path modelPath(const fs::path& modelsDir, int id) {
    return modelsDir / (std::string(kModelFilePrefix) + std::to_string(id) + ".xml");
}

bool checkXMLFileExists(const fs::path& pathToFile) {
    std::error_code ec;
    return fs::is_regular_file(pathToFile, ec);
}

std::vector<cv::String> getFaceModelsFiles(const fs::path& modelsDir) {
    std::vector<cv::String> filesFaceModels;
    std::error_code ec;
    if (!fs::is_directory(modelsDir, ec)) {
        return filesFaceModels;
    }
    for (const auto& entry : fs::directory_iterator(modelsDir, ec)) {
        const std::string name = entry.path().filename().string();
        const bool isModel = entry.is_regular_file() && name.starts_with(kModelFilePrefix) && name.ends_with(".xml");
        if (isModel) {
            filesFaceModels.push_back(entry.path().string());
        }
    }
    return filesFaceModels;
}
