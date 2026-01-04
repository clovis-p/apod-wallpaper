#include "FsUtils.h"

#include <filesystem>
#include <iostream>

std::string FsUtils::cacheDir;

std::string FsUtils::getCacheDir() {
    char *homeDir = std::getenv("HOME");
    if (homeDir == nullptr) {
        std::cout << "Warning: Couldn't get home directory ($HOME is empty). Falling back to /tmp." << std::endl;
        cacheDir = "/tmp/apod-fetcher";
    }
    else {
        cacheDir = std::string(homeDir) + "/.cache/apod-fetcher";
    }

    if (!std::filesystem::exists(cacheDir)) {
        std::cout << "Creating cache dir at " << cacheDir << std::endl;
        std::filesystem::create_directories(cacheDir);
    }

    return cacheDir;
}
