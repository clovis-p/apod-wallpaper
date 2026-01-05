#ifndef APOD_WALLPAPER_WINDOWMANAGER_H
#define APOD_WALLPAPER_WINDOWMANAGER_H

#include <gtk/gtk.h>

#include "ApodFetcher.h"
#include "BackgroundChanger.h"


class WindowController {
public:
    WindowController(ApodFetcher *apodFetcher, BackgroundChanger *backgroundChanger);

    int initWindow();

private:
    static void activate(GtkApplication *app, gpointer data);
    static void onRefreshButtonClick(GtkApplication *app);
    static void onPreviousButtonClick();
    static void onNextButtonClick();
    static void onDateEntryIconClick(GtkEntry *entry, GtkEntryIconPosition pos, gpointer data);
    static void onCalendarDateClick(GtkCalendar *cal, gpointer data);
    static void refreshDateEntry();
    static void onSetAsWallpaperButtonClick(GtkApplication *app);
    static void refreshApodImageView();
    static void onWindowClose();
    static void mainLoop(GMainContext *context);
    static bool validateDateString(std::string dateStr);
    static std::chrono::year_month_day dateStringToDate(std::string dateStr);

    static GtkWindow *window;
    static ApodFetcher *apodFetcher_;
    static BackgroundChanger *backgroundChanger_;
    static GtkButton *refreshButton;
    static GtkButton *previousButton;
    static GtkButton *nextButton;
    static GtkEntry *dateEntry;
    static GtkPopover *calendarPopover;
    static GtkCalendar *calendar;
    static GtkButton *setAsWallpaperButton;
    static GtkSpinner *spinner;
    static GtkPicture *image;
    static bool quit;
};


#endif //APOD_WALLPAPER_WINDOWMANAGER_H