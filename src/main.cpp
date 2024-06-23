#include <opencv2/core/utils/logger.hpp>

#include <exception>
#include <iostream>
#include <limits>
#include <optional>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "facerec/config.hpp"
#include "facerec/enroller.hpp"
#include "facerec/model_store.hpp"
#include "facerec/recognizer.hpp"

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
            return config;  // command stays Menu: nothing to do
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
    const ModelStore store(config.modelsDir);
    const std::vector<int> ids = store.listIds();
    if (ids.empty()) {
        std::cout << "В каталоге " << store.dir().string() << " нет моделей\n";
        return kExitOk;
    }
    std::cout << "Модели в " << store.dir().string() << ":\n";
    for (const int id : ids) {
        std::cout << "  ID " << id << "\t" << store.pathFor(id).filename().string() << '\n';
    }
    return kExitOk;
}

int runCommand(const Config& config) {
    switch (config.command) {
        case Command::Recognize:
            return runRecognition(config);
        case Command::Enroll:
            return runEnroll(config, EnrollMode::Create);
        case Command::Update:
            return runEnroll(config, EnrollMode::Update);
        case Command::List:
            return runList(config);
        case Command::Help:
        case Command::Menu:
            std::cout << usage();
            return kExitOk;
    }
    return kExitOk;
}

}  // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
    // Messages are UTF-8 (/utf-8 in CMake); make the Windows console display them correctly.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
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
        config = *chosen;
        if (config.command == Command::Menu) {
            return kExitOk;  // "0. Выход"
        }
    }

    try {
        return runCommand(config);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return kExitRuntime;
    }
}
