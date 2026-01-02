#include <iostream>

#include "ApodFetcher.h"
#include "BackgroundChanger.h"

int main() {
    std::string url = "https://apod.nasa.gov";

    auto* apodFetcher = new ApodFetcher(url);
    apodFetcher->fetchApod();

    auto* backgroundChanger = new BackgroundChanger(SWAYBG);
    backgroundChanger->ChangeBackground();

    return 0;
}