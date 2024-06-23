#pragma once

#include <opencv2/core/cvstd.hpp>

#include <filesystem>
#include <string_view>
#include <string>
#include <vector>

inline constexpr std::string_view kModelFilePrefix = "face_model_ID_";

bool ensureDirectory(const std::filesystem::path& dir);

std::filesystem::path modelPath(const std::filesystem::path& modelsDir, int id);

bool checkXMLFileExists(const std::filesystem::path& pathToFile);

std::vector<cv::String> getFaceModelsFiles(const std::filesystem::path& modelsDir);
