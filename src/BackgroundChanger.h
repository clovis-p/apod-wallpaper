#ifndef APOD_WALLPAPER_BACKGROUNDCHANGER_H
#define APOD_WALLPAPER_BACKGROUNDCHANGER_H
#include <string>

enum Strategy {
    GNOME,
    KDE,
    SWAYBG
};

class BackgroundChanger {
private:
    Strategy strategy_;
public:
    BackgroundChanger(Strategy strategy);

    void ChangeBackground();
};


#endif //APOD_WALLPAPER_BACKGROUNDCHANGER_H