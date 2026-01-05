#ifndef APOD_WALLPAPER_APODFETCHER_H
#define APOD_WALLPAPER_APODFETCHER_H
#include <chrono>
#include <string>
#include <httplib.h>

class ApodFetcher {
private:
    httplib::Client httpClient;
    std::string fetchApodImagePath(std::chrono::year_month_day date);
    std::string fetchApodImage(const std::string &remotePath, std::chrono::year_month_day);
    std::string apodImageExtension;

public:
    ApodFetcher(const std::string &url);

    std::string fetchApod();
    std::string fetchApod(std::chrono::year_month_day date);

    static std::string dateToFormattedString(std::chrono::year_month_day date);
    static std::string dateToWallpaperPath(std::chrono::year_month_day date);
};


#endif //APOD_WALLPAPER_APODFETCHER_H