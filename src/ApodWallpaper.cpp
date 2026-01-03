#include <iostream>

#include "ApodFetcher.h"
#include "BackgroundChanger.h"

int main() {
    std::string url = "https://apod.nasa.gov";

    auto* apodFetcher = new ApodFetcher(url);
    std::string localImagePath = apodFetcher->fetchApod();

    if (!localImagePath.empty()) {
        auto* backgroundChanger = new BackgroundChanger(SWAYBG);
        backgroundChanger->ChangeBackground();
    }
    else {
        std::cerr << "Failed to download APOD" << std::endl;
        return -1;
    }

    return 0;
}