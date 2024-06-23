#include "facerec/model_store.hpp"

#include <algorithm>
#include <charconv>
#include <iostream>
#include <string_view>
#include <system_error>
#include <utility>

namespace facerec {

namespace fs = std::filesystem;

namespace {
constexpr std::string_view kPrefix = "face_model_ID_";
constexpr std::string_view kExtension = ".xml";
}  // namespace

std::optional<int> parseModelFileName(const std::string& fileName) {
    const std::string_view name(fileName);
    if (!name.starts_with(kPrefix) || !name.ends_with(kExtension)) {
        return std::nullopt;
    }
    const std::string_view digits = name.substr(kPrefix.size(), name.size() - kPrefix.size() - kExtension.size());
    int id = -1;
    const auto [ptr, ec] = std::from_chars(digits.data(), digits.data() + digits.size(), id);
    if (digits.empty() || ec != std::errc() || ptr != digits.data() + digits.size() || id < 0) {
        return std::nullopt;
    }
    return id;
}

ModelStore::ModelStore(fs::path dir) : dir_(std::move(dir)) {}

fs::path ModelStore::pathFor(int id) const {
    return dir_ / (std::string(kPrefix) + std::to_string(id) + std::string(kExtension));
}

bool ModelStore::exists(int id) const {
    std::error_code ec;
    return fs::is_regular_file(pathFor(id), ec);
}

std::vector<int> ModelStore::listIds() const {
    std::vector<int> ids;
    std::error_code ec;
    if (!fs::is_directory(dir_, ec)) {
        return ids;
    }
    for (const auto& entry : fs::directory_iterator(dir_, ec)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        if (const auto id = parseModelFileName(entry.path().filename().string())) {
            ids.push_back(*id);
        }
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

LbphModel ModelStore::load(int id) const {
    const fs::path path = pathFor(id);
    try {
        LbphModel model = cv::face::LBPHFaceRecognizer::create();
        model->read(path.string());
        if (model->empty()) {
            std::cerr << "Модель пуста или повреждена: " << path.string() << '\n';
            return nullptr;
        }
        return model;
    } catch (const cv::Exception& e) {
        std::cerr << "Не удалось прочитать модель " << path.string() << ": " << e.what() << '\n';
        return nullptr;
    }
}

std::vector<PersonModel> ModelStore::loadAll() const {
    std::vector<PersonModel> models;
    for (const int id : listIds()) {
        if (LbphModel model = load(id)) {
            models.push_back({id, std::move(model)});
        }
    }
    return models;
}

bool ModelStore::save(int id, const LbphModel& model) const {
    std::error_code ec;
    fs::create_directories(dir_, ec);
    if (ec) {
        std::cerr << "Не удалось создать каталог " << dir_.string() << ": " << ec.message() << '\n';
        return false;
    }
    const fs::path path = pathFor(id);
    try {
        model->save(path.string());
    } catch (const cv::Exception& e) {
        std::cerr << "Не удалось сохранить модель " << path.string() << ": " << e.what() << '\n';
        return false;
    }
    return true;
}

}  // namespace facerec
