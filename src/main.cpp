#include <opencv2/core/utils/logger.hpp>

#include <iostream>
#include <limits>
#include <optional>

#include "facerec/FaceModelTrainer.hpp"
#include "facerec/FaceRecognition.hpp"
#include "facerec/FileOperations.hpp"
#include "facerec/config.hpp"

using namespace facerec;

namespace {

// Reads a non-negative integer; on bad input returns false instead of leaving garbage in the variable.
bool readNonNegativeInt(int& value) {
    if (!(std::cin >> value) || value < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cerr << "Ожидалось целое неотрицательное число\n";
        return false;
    }
    return true;
}

// The original text menu, kept for launching without arguments.
std::optional<Config> askInteractive(Config config) {
    std::cout << "1. Распознавание лица\n"
                 "2. Добавить модель своего лица\n"
                 "3. Дообучить уже имеющуюся модель лица\n"
                 "4. Показать сохранённые модели\n"
                 "0. Выход\n"
                 "> ";
    int choice = 0;
    if (!readNonNegativeInt(choice)) {
        return std::nullopt;
    }
    switch (choice) {
        case 1:
            config.command = Command::Recognize;
            break;
        case 2:
            config.command = Command::Enroll;
            break;
        case 3:
            config.command = Command::Update;
            break;
        case 4:
            config.command = Command::List;
            break;
        case 0:
            config.command = Command::Help;
            return config;
        default:
            std::cerr << "Нет такого пункта меню\n";
            return std::nullopt;
    }

    if (config.command == Command::Enroll || config.command == Command::Update) {
        std::cout << "Введите ваш уникальный ID: ";
        if (!readNonNegativeInt(config.personId)) {
            return std::nullopt;
        }
    }
    return config;
}

int runList(const Config& config) {
    const auto files = getFaceModelsFiles(config.modelsDir);
    if (files.empty()) {
        std::cout << "В каталоге " << config.modelsDir.string() << " нет моделей\n";
        return kExitOk;
    }
    std::cout << "Модели в " << config.modelsDir.string() << ":\n";
    for (const auto& file : files) {
        std::cout << "  " << file << '\n';
    }
    return kExitOk;
}

int runCommand(const Config& config) {
    if (config.command == Command::List) {
        return runList(config);
    }

    const auto cascade = resolveCascadePath(config);
    if (!cascade) {
        std::cerr << "Не найден haarcascade_frontalface_default.xml. Укажите путь через --cascade "
                     "или переменную окружения FACEREC_CASCADE\n";
        return kExitRuntime;
    }

    FaceModelTrainer faceModel(cascade->string(), config.cameraIndex, config.samplesPerEnroll);
    const auto path = modelPath(config.modelsDir, config.personId);

    switch (config.command) {
        case Command::Recognize: {
            const auto files = getFaceModelsFiles(config.modelsDir);
            if (files.empty()) {
                std::cerr << "Нет ни одной модели в " << config.modelsDir.string()
                          << ". Сначала добавьте лицо: facerec enroll --id N\n";
                return kExitRuntime;
            }
            faceModel.loadModels(files);
            FaceRecognition faceRecognition(&faceModel, cascade->string(), config.cameraIndex, config.threshold);
            return faceRecognition.recognizeFaces() ? kExitOk : kExitRuntime;
        }
        case Command::Enroll:
            if (checkXMLFileExists(path)) {
                std::cerr << "ID " << config.personId << " уже существует. Дообучить: facerec update --id "
                          << config.personId << '\n';
                return kExitUsage;
            }
            if (!ensureDirectory(config.modelsDir) || !faceModel.captureAndAddFace(config.personId) ||
                !faceModel.trainNewModel(path.string())) {
                return kExitRuntime;
            }
            std::cout << "Модель сохранена: " << path.string() << '\n';
            return kExitOk;
        case Command::Update:
            if (!checkXMLFileExists(path)) {
                std::cerr << "ID " << config.personId << " не существует. Добавить: facerec enroll --id "
                          << config.personId << '\n';
                return kExitUsage;
            }
            if (!faceModel.captureAndAddFace(config.personId) || !faceModel.updateModel(path.string())) {
                return kExitRuntime;
            }
            std::cout << "Модель обновлена: " << path.string() << '\n';
            return kExitOk;
        default:
            return kExitOk;
    }
}

}  // namespace

int main(int argc, char** argv) {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);

    const ParseResult parsed = parseArgs(argc, argv);
    if (!parsed.config) {
        std::cerr << "Ошибка: " << parsed.error << "\nСправка: facerec --help\n";
        return kExitUsage;
    }

    Config config = *parsed.config;
    if (config.command == Command::Menu) {
        const auto chosen = askInteractive(config);
        if (!chosen) {
            return kExitUsage;
        }
        if (chosen->command == Command::Help) {
            return kExitOk;  // "0. Выход"
        }
        config = *chosen;
    }
    if (config.command == Command::Help) {
        std::cout << usage();
        return kExitOk;
    }

    return runCommand(config);
}
