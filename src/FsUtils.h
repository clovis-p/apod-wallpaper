//
// Created by clovis on 1/3/26.
//

#ifndef APOD_WALLPAPER_CACHEUTILS_H
#define APOD_WALLPAPER_CACHEUTILS_H
#include <string>


class FsUtils {
public:
    static std::string getCacheDir();

private:
    static std::string cacheDir;
};


#endif //APOD_WALLPAPER_CACHEUTILS_H