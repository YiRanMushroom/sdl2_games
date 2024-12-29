module;

#include <filesystem>
#include <fstream>
#include <tuple>

export module fileUtilities;

void createDirectoryIfAbsent(const std::filesystem::path &path) {
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directory(path);
    }
}

export void createFile(const std::filesystem::path &path) {
    createDirectoryIfAbsent(path.parent_path());
    std::ofstream fileStream;
    fileStream.open(path);
    fileStream.close();
}

export std::pair<bool, std::ifstream> openFileInput(const std::filesystem::path &path) {
    bool exists = std::filesystem::exists(path);
    if (!exists) {
        createFile(path);
    }
    std::ifstream (fileStream);
    fileStream.open(path);
    return {exists, std::move(fileStream)};
}

export std::pair<bool, std::ofstream> openFileOutput(const std::filesystem::path &path) {
    bool exists = std::filesystem::exists(path);
    if (!exists) {
        createFile(path);
    }
    std::ofstream (fileStream);
    fileStream.open(path);
    return {exists, std::move(fileStream)};
}
