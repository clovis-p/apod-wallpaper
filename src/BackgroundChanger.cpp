#include "BackgroundChanger.h"

#include <filesystem>
#include <iostream>

#include "FsUtils.h"

BackgroundChanger::BackgroundChanger(Strategy strategy) {
    strategy_ = strategy;
    if (strategy == GNOME || strategy == KDE) {
        std::cerr << "Strategy " << strategy << " not implemented" << std::endl;
    }
}

void BackgroundChanger::changeBackground(const std::string &dirPath) {
    if (strategy_ == GNOME || strategy_ == KDE) {
        std::cerr << "Strategy " << strategy_ << " not implemented" << std::endl;

        return;
    }

    std::string wallpaperPath = FsUtils::getCacheDir() + "/wallpaper/" + dirPath;
    for (const auto& entry : std::filesystem::directory_iterator(wallpaperPath)) {
        if (entry.is_regular_file()) {
            wallpaperPath = entry.path();
        }
    }

    if (strategy_ == SWAYBG) {
        const std::string killSwayBgCommand = "pkill -x swaybg";
        std::cout << "Running \"" + killSwayBgCommand + "\"" << std::endl;
        system(killSwayBgCommand.c_str());

        const std::string changeBgCommand = "swaybg -i " + wallpaperPath + " -m fill &";
        std::cout << "Running \"" + changeBgCommand + "\"" << std::endl;
        system(changeBgCommand.c_str());
    }
}