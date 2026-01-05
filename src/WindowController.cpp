#include <gtk/gtk.h>

#include "WindowController.h"

#include "BackgroundChanger.h"

ApodFetcher* WindowController::apodFetcher_ = nullptr;
BackgroundChanger* WindowController::backgroundChanger_ = nullptr;
bool WindowController::quit = false;
GtkWindow* WindowController::window = nullptr;
GtkButton *WindowController::previousButton = nullptr;
GtkButton *WindowController::nextButton = nullptr;
GtkButton *WindowController::refreshButton = nullptr;
GtkEntry *WindowController::dateEntry = nullptr;
GtkPopover *WindowController::calendarPopover = nullptr;
GtkCalendar *WindowController::calendar = nullptr;
GtkButton *WindowController::setAsWallpaperButton = nullptr;
GtkSpinner *WindowController::spinner = nullptr;
GtkPicture *WindowController::image = nullptr;

WindowController::WindowController(ApodFetcher *apodFetcher, BackgroundChanger *backgroundChanger) {
    apodFetcher_ = apodFetcher;
    backgroundChanger_ = backgroundChanger;
}

int WindowController::initWindow() {
    GtkApplication *app = gtk_application_new("lol.clovis.apod-wallpaper", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(WindowController::activate), NULL);

    GError *error = nullptr;
    g_application_register(G_APPLICATION(app), nullptr, &error);
    if (error != nullptr) {
        g_printerr("Failed to register: %s\n", error->message);
        g_error_free(error);
        return -1;
    }

    g_application_activate(G_APPLICATION(app));
    GMainContext *context = g_main_context_default();
    while (!quit) {
        mainLoop(context);
    }

    g_object_unref(app);

    return 0;
}

void WindowController::activate(GtkApplication *app, gpointer data)
{
    GtkBuilder *gtk_builder = gtk_builder_new_from_file("../view/view.ui");

    window = GTK_WINDOW(gtk_builder_get_object(gtk_builder, "main-window"));
    gtk_window_set_application(window, app);

    refreshButton = GTK_BUTTON(gtk_builder_get_object(gtk_builder, "refresh-button"));
    previousButton = GTK_BUTTON(gtk_builder_get_object(gtk_builder, "previous-button"));
    nextButton = GTK_BUTTON(gtk_builder_get_object(gtk_builder, "next-button"));
    spinner = GTK_SPINNER(gtk_builder_get_object(gtk_builder, "spinner"));
    dateEntry = GTK_ENTRY(gtk_builder_get_object(gtk_builder, "date-entry"));
    calendarPopover = GTK_POPOVER(gtk_builder_get_object(gtk_builder, "calendar-popover"));
    calendar = GTK_CALENDAR(gtk_builder_get_object(gtk_builder, "calendar"));
    setAsWallpaperButton = GTK_BUTTON(gtk_builder_get_object(gtk_builder, "set-as-wallpaper-button"));
    image = GTK_PICTURE(gtk_builder_get_object(gtk_builder, "image"));

    g_signal_connect(refreshButton, "clicked", G_CALLBACK(WindowController::onRefreshButtonClick), spinner);
    g_signal_connect(previousButton, "clicked", G_CALLBACK(WindowController::onPreviousButtonClick), nullptr);
    g_signal_connect(nextButton, "clicked", G_CALLBACK(WindowController::onNextButtonClick), nullptr);
    g_signal_connect(dateEntry, "icon-press", G_CALLBACK(WindowController::onDateEntryIconClick), calendarPopover);
    g_signal_connect(calendar, "day-selected", G_CALLBACK(WindowController::onCalendarDateClick), dateEntry);
    g_signal_connect(setAsWallpaperButton, "clicked", G_CALLBACK(WindowController::onSetAsWallpaperButtonClick), spinner);
    g_signal_connect(window, "destroy", G_CALLBACK(WindowController::onWindowClose), nullptr);

    gtk_window_present(window);

    g_object_unref(gtk_builder);

    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "../view/style.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    refreshDateEntry();
    refreshApodImageView();
}

void WindowController::onRefreshButtonClick(GtkApplication *app) {
    refreshApodImageView();
}

void WindowController::onPreviousButtonClick() {
    gtk_calendar_set_date(calendar, g_date_time_add_days(gtk_calendar_get_date(calendar), -1));
    // No need to refresh the date entry, gtk_calendar_set_date invokes onCalendarDateClick
}

void WindowController::onNextButtonClick() {
    gtk_calendar_set_date(calendar, g_date_time_add_days(gtk_calendar_get_date(calendar), 1));
    // No need to refresh the date entry, gtk_calendar_set_date invokes onCalendarDateClick
}

void WindowController::onDateEntryIconClick(GtkEntry *entry, GtkEntryIconPosition pos, gpointer data) {
    if (pos == GTK_ENTRY_ICON_SECONDARY) {
        GtkPopover *popover = GTK_POPOVER(data);
        gtk_popover_popup(popover);
    }
}

void WindowController::onCalendarDateClick(GtkCalendar *cal, gpointer data) {
    refreshDateEntry();
    refreshApodImageView();
}

void WindowController::refreshDateEntry() {
    GDateTime *date = gtk_calendar_get_date(calendar);
    char *dateStr = g_date_time_format(date, "%Y/%m/%d");
    gtk_editable_set_text(GTK_EDITABLE(dateEntry), dateStr);
    g_free(dateStr);
}

void WindowController::onSetAsWallpaperButtonClick(GtkApplication *app) {
    std::thread([]() {
        const char* dateChars = gtk_editable_get_text(GTK_EDITABLE(dateEntry));
        const std::string dateStr = std::string(dateChars);

        if (validateDateString(dateStr)) {
            auto date = dateStringToDate(dateStr);
            std::string path = ApodFetcher::dateToWallpaperPath(date);
            backgroundChanger_->changeBackground(path);
        }
    }).detach();
}

void WindowController::refreshApodImageView() {
    // todo: stop thread on click instead of disabling buttons
    gtk_widget_set_sensitive(GTK_WIDGET(refreshButton), false);
    gtk_widget_set_sensitive(GTK_WIDGET(nextButton), false);
    gtk_widget_set_sensitive(GTK_WIDGET(previousButton), false);
    gtk_spinner_start(spinner);

    std::thread([]() {
        const char* dateChars = gtk_editable_get_text(GTK_EDITABLE(dateEntry));
        const std::string dateStr = std::string(dateChars);
        std::chrono::year_month_day ymd{};

        if (validateDateString(dateStr)) {
            ymd = dateStringToDate(dateStr);
        }
        else {
            std::cout << "Invalid date format" << std::endl;
            gtk_spinner_stop(spinner);
            gtk_widget_set_sensitive(GTK_WIDGET(refreshButton), true);
            gtk_widget_set_sensitive(GTK_WIDGET(nextButton), true);
            gtk_widget_set_sensitive(GTK_WIDGET(previousButton), true);

            return;
        }

        char* imagePath = g_strdup(apodFetcher_->fetchApod(ymd).c_str());

        g_main_context_invoke(g_main_context_default(), [](gpointer data) -> gboolean {
            if (quit) {
                g_free(data);
                return G_SOURCE_REMOVE;
            }

            char* p = static_cast<char*>(data);

            int len = std::strlen(p);
            if (len > 0) {
                std::cout << "Setting APOD view to " << p << std::endl;
                gtk_picture_set_filename(image, p);
                gtk_widget_set_sensitive(GTK_WIDGET(setAsWallpaperButton), true);
                gtk_widget_set_sensitive(GTK_WIDGET(nextButton), true);
                gtk_widget_set_sensitive(GTK_WIDGET(previousButton), true);
                }
            else {
                std::cerr << "Skipping gtk_picture update: apodFetcher returned an empty string." << std::endl;
            }

            gtk_spinner_stop(spinner);
            gtk_widget_set_sensitive(GTK_WIDGET(refreshButton), true);
            gtk_widget_set_sensitive(GTK_WIDGET(nextButton), true);
            gtk_widget_set_sensitive(GTK_WIDGET(previousButton), true);

            g_free(p);

            return G_SOURCE_REMOVE;
        }, imagePath);
    }).detach();
}

void WindowController::onWindowClose() {
    quit = true;
}

void WindowController::mainLoop(GMainContext *context) {
    g_main_context_iteration(context, true);
}

bool WindowController::validateDateString(std::string dateStr) {
    auto dateStrRegex = std::regex(R"(^[0-9]{4}/[0-9]{1,2}/[0-9]{1,2}$)", std::regex_constants::ECMAScript);

    auto match = std::sregex_iterator(dateStr.begin(), dateStr.end(), dateStrRegex);
    auto matchCount = std::distance(match, std::sregex_iterator());

    return matchCount == 1;
}

// validate before using
std::chrono::year_month_day WindowController::dateStringToDate(std::string dateStr) {
    auto dateSS = std::stringstream(dateStr);
    std::string y;
    std::string m;
    std::string d;
    getline(dateSS, y, '/');
    getline(dateSS, m, '/');
    getline(dateSS, d, '/');

    return std::chrono::year_month_day(std::chrono::year(stoi(y)), std::chrono::month(stoi(m)), std::chrono::day(stoi(d)));
}
