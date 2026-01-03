//
// Created by clovis on 1/3/26.
//

#include "FsUtils.h"

#include <filesystem>
#include <iostream>

std::string FsUtils::cacheDir;

std::string FsUtils::getCacheDir() {
    std::string homeDir = std::getenv("HOME");
    if (homeDir.empty()) {
        std::cout << "Warning: Couldn't get home directory ($HOME is empty). Falling back to /tmp." << std::endl;
        cacheDir = "/tmp/apod-fetcher";
    }
    else {
        cacheDir = homeDir + "/.cache/apod-fetcher";
    }

    if (!std::filesystem::exists(cacheDir)) {
        std::cout << "Creating cache dir at " << cacheDir << std::endl;
        std::filesystem::create_directories(cacheDir);
    }

    return cacheDir;
}
