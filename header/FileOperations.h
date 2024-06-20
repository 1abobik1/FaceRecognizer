#pragma once

#include <string>
#include <vector>
#include <opencv2/core/cvstd.hpp>

static const std::string PATH_TO_FACEMODELS_DIR = "C:/Users/dima1/source/repos/Facerecognizer/FaceModels/";
static const std::string FILE_NAME = "face_model_ID_";
static const std::string PATH_TO_PROJECT = "C:/Users/dima1/source/repos/Facerecognizer/";

void createFolder(const std::string& folderPath);

void createXMLFile(const std::string& newFile, const int label);

bool checkXMLFileExists(const std::string& fileName);

std::vector<cv::String> getFaceModelsFiles(const std::string& dirFaceModels);