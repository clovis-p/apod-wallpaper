#include <iostream>

#include "ApodFetcher.h"

int main() {
    std::string url = "https://apod.nasa.gov";

    ApodFetcher* apod_fetcher = new ApodFetcher(url);

    std::string imagePath = apod_fetcher->fetchApod();

    std::cout << imagePath;

    return 0;
}