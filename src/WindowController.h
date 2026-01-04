#ifndef APOD_WALLPAPER_WINDOWMANAGER_H
#define APOD_WALLPAPER_WINDOWMANAGER_H

#include <gtk/gtk.h>

#include "ApodFetcher.h"


class WindowController {
public:
    WindowController(ApodFetcher *apodFetcher);

    int initWindow();

private:
    static void activate(GtkApplication *app, gpointer data);
    static void onRefreshButtonClick(GtkApplication *app);
    static void refreshApodImageView();
    static void onWindowClose();
    static void mainLoop(GMainContext *context);

    static GtkWindow *window;
    static ApodFetcher *apodFetcher_;
    static GtkButton *refreshButton;
    static GtkSpinner *spinner;
    static GtkPicture *image;
    static bool quit;
};


#endif //APOD_WALLPAPER_WINDOWMANAGER_H