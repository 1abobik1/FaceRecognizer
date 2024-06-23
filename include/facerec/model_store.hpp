#pragma once

#include <opencv2/core.hpp>
#include <opencv2/face.hpp>

#include <filesystem>
#include <optional>
#include <vector>

namespace facerec {

using LbphModel = cv::Ptr<cv::face::LBPHFaceRecognizer>;

struct PersonModel {
    int id = -1;
    LbphModel model;
};

// One LBPH model per person, stored as <dir>/face_model_ID_<id>.xml.
class ModelStore {
public:
    explicit ModelStore(std::filesystem::path dir);

    const std::filesystem::path& dir() const { return dir_; }
    std::filesystem::path pathFor(int id) const;
    bool exists(int id) const;

    // IDs of all model files in the directory, sorted.
    std::vector<int> listIds() const;

    // Returns nullptr (and prints the reason) if the file is missing or broken.
    LbphModel load(int id) const;

    // Loads every model; broken files are skipped with a warning.
    std::vector<PersonModel> loadAll() const;

    bool save(int id, const LbphModel& model) const;

private:
    std::filesystem::path dir_;
};

// "face_model_ID_42.xml" -> 42; anything else -> nullopt.
std::optional<int> parseModelFileName(const std::string& fileName);

}  // namespace facerec
