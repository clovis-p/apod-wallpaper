#include <iostream>

#include "ApodFetcher.h"
#include "BackgroundChanger.h"
#include "WindowController.h"

int main() {
    std::string url = "https://apod.nasa.gov";
    auto* apodFetcher = new ApodFetcher(url);

    auto *windowController = new WindowController(apodFetcher);
    windowController->initWindow();

    // todo: move this to a UI button signal handler
    ////std::string localImagePath = apodFetcher->fetchApod();
    //std::string localImagePath;

    //if (!localImagePath.empty()) {
    //    auto* backgroundChanger = new BackgroundChanger(SWAYBG);
    //    backgroundChanger->changeBackground();
    //}
    //else {
    //    std::cerr << "Failed to download APOD" << std::endl;
    //    return -1;
    //}

    return 0;
}
