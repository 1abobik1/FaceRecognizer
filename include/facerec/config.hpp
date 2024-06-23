#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace facerec {

enum class Command { Menu, Recognize, Enroll, Update, List, Help };

enum ExitCode : int {
    kExitOk = 0,
    kExitUsage = 1,    // bad arguments or input
    kExitRuntime = 2,  // camera, cascade or model file problems
};

struct Config {
    Command command = Command::Menu;
    std::filesystem::path modelsDir = "FaceModels";
    std::filesystem::path cascadePath;    // empty: search automatically, see resolveCascadePath()
    std::filesystem::path executableDir;  // used to find data/ next to the binary
    int cameraIndex = 0;
    int personId = -1;
    int samplesPerEnroll = 20;
    double threshold = 50.0;  // LBPH distance: lower means more similar
};

struct ParseResult {
    std::optional<Config> config;
    std::string error;  // set when config is empty
};

ParseResult parseArgs(int argc, char** argv);

std::string usage();

// Search order: --cascade, $FACEREC_CASCADE, data/ next to the binary, data/ in the working directory,
// data/ in the source tree (baked in at build time), OpenCV's own samples data.
std::optional<std::filesystem::path> resolveCascadePath(const Config& config);

}  // namespace facerec
