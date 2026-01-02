#ifndef APOD_WALLPAPER_APODFETCHER_H
#define APOD_WALLPAPER_APODFETCHER_H
#include <string>
#include <httplib.h>

class ApodFetcher {
private:
    std::string cacheDir;
    httplib::Client httpClient;
    std::string fetchApodImagePath();
    std::string fetchApodImage(const std::string &remotePath);
    std::string apodImageExtension;

public:
    ApodFetcher(const std::string &url);

    std::string fetchApod();
};


#endif //APOD_WALLPAPER_APODFETCHER_H