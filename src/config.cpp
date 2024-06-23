#include "facerec/config.hpp"

#include <opencv2/core/utility.hpp>

#include <charconv>
#include <cstdlib>
#include <string_view>
#include <system_error>
#include <vector>

namespace facerec {

namespace fs = std::filesystem;

namespace {

constexpr std::string_view kCascadeRelPath = "haarcascades/haarcascade_frontalface_default.xml";

bool parseInt(std::string_view text, int& out) {
    const char* end = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(text.data(), end, out);
    return ec == std::errc() && ptr == end;
}

bool parseDouble(const std::string& text, double& out) {
    char* end = nullptr;
    out = std::strtod(text.c_str(), &end);
    return !text.empty() && end == text.c_str() + text.size();
}

std::optional<Command> parseCommand(std::string_view name) {
    if (name == "recognize") {
        return Command::Recognize;
    }
    if (name == "enroll") {
        return Command::Enroll;
    }
    if (name == "update") {
        return Command::Update;
    }
    if (name == "list") {
        return Command::List;
    }
    if (name == "help") {
        return Command::Help;
    }
    return std::nullopt;
}

fs::path executableDir(const char* argv0) {
    std::error_code ec;
    // Linux: the real binary path, even when started via PATH.
    const fs::path self = fs::read_symlink("/proc/self/exe", ec);
    if (!ec) {
        return self.parent_path();
    }
    if (argv0 == nullptr || *argv0 == '\0') {
        return {};
    }
    const fs::path absolute = fs::absolute(argv0, ec);
    return ec ? fs::path() : absolute.parent_path();
}

ParseResult fail(std::string message) {
    return {std::nullopt, std::move(message)};
}

}  // namespace

ParseResult parseArgs(int argc, char** argv) {
    Config config;
    config.executableDir = executableDir(argc > 0 ? argv[0] : nullptr);

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    std::size_t i = 0;
    if (!args.empty() && !args[0].starts_with("-")) {
        const auto command = parseCommand(args[0]);
        if (!command) {
            return fail("неизвестная команда: " + args[0]);
        }
        config.command = *command;
        i = 1;
    }

    for (; i < args.size(); ++i) {
        const std::string& option = args[i];
        if (option == "-h" || option == "--help") {
            config.command = Command::Help;
            continue;
        }

        const bool knownOption = option == "--id" || option == "--samples" || option == "--camera" ||
                                 option == "--threshold" || option == "--models-dir" || option == "--cascade";
        if (!knownOption) {
            return fail("неизвестный параметр: " + option);
        }
        if (i + 1 >= args.size()) {
            return fail("не указано значение для " + option);
        }
        const std::string& value = args[++i];

        if (option == "--id") {
            if (!parseInt(value, config.personId) || config.personId < 0) {
                return fail("--id должен быть целым числом >= 0");
            }
        } else if (option == "--samples") {
            if (!parseInt(value, config.samplesPerEnroll) || config.samplesPerEnroll <= 0) {
                return fail("--samples должен быть целым числом > 0");
            }
        } else if (option == "--camera") {
            if (!parseInt(value, config.cameraIndex) || config.cameraIndex < 0) {
                return fail("--camera должен быть целым числом >= 0");
            }
        } else if (option == "--threshold") {
            if (!parseDouble(value, config.threshold) || config.threshold <= 0.0) {
                return fail("--threshold должен быть числом > 0");
            }
        } else if (option == "--models-dir") {
            config.modelsDir = value;
        } else if (option == "--cascade") {
            config.cascadePath = value;
        }
    }

    const bool needsId = config.command == Command::Enroll || config.command == Command::Update;
    if (needsId && config.personId < 0) {
        return fail("для команд enroll и update нужен параметр --id N");
    }
    return {config, {}};
}

std::string usage() {
    return R"(Использование: facerec [команда] [параметры]

Команды:
  recognize          распознавание лиц с камеры (Esc или q - выход)
  enroll --id N      добавить новое лицо с идентификатором N
  update --id N      дообучить модель лица N новыми снимками
  list               показать сохранённые модели
  help               эта справка
  (без команды)      интерактивное меню

Параметры:
  --id N             идентификатор человека, целое число >= 0
  --samples N        сколько снимков лица сделать (по умолчанию 20)
  --camera N         номер камеры (по умолчанию 0)
  --threshold T      порог расстояния LBPH, меньше - строже (по умолчанию 50)
  --models-dir DIR   каталог с моделями (по умолчанию ./FaceModels)
  --cascade FILE     путь к haarcascade_frontalface_default.xml
  -h, --help         эта справка

Переменные окружения:
  FACEREC_CASCADE    путь к каскаду Хаара, если он не лежит рядом с программой
)";
}

std::optional<fs::path> resolveCascadePath(const Config& config) {
    std::error_code ec;
    if (!config.cascadePath.empty()) {
        // An explicit path is not silently replaced by another file.
        if (fs::is_regular_file(config.cascadePath, ec)) {
            return config.cascadePath;
        }
        return std::nullopt;
    }

    std::vector<fs::path> candidates;
    if (const char* env = std::getenv("FACEREC_CASCADE"); env != nullptr && *env != '\0') {
        candidates.emplace_back(env);
    }
    if (!config.executableDir.empty()) {
        candidates.push_back(config.executableDir / "data" / kCascadeRelPath);
    }
    candidates.push_back(fs::path("data") / kCascadeRelPath);
#ifdef FACEREC_DATA_DIR
    candidates.push_back(fs::path(FACEREC_DATA_DIR) / kCascadeRelPath);
#endif

    for (const auto& candidate : candidates) {
        if (fs::is_regular_file(candidate, ec)) {
            return candidate;
        }
    }

    const std::string found = cv::samples::findFile(std::string(kCascadeRelPath), false, true);
    if (!found.empty()) {
        return fs::path(found);
    }
    return std::nullopt;
}

}  // namespace facerec
