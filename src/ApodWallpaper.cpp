#include <iostream>

#include "ApodFetcher.h"
#include "BackgroundChanger.h"
#include "WindowController.h"

int main() {
    std::string url = "https://apod.nasa.gov";
    auto* apodFetcher = new ApodFetcher(url);

    auto* backgroundChanger = new BackgroundChanger(SWAYBG);

    auto *windowController = new WindowController(apodFetcher, backgroundChanger);
    windowController->initWindow();

    return 0;
}
