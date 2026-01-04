#include <gtk/gtk.h>

#include "WindowController.h"

#include "BackgroundChanger.h"

ApodFetcher* WindowController::apodFetcher_ = nullptr;
BackgroundChanger* WindowController::backgroundChanger_ = nullptr;
bool WindowController::quit = false;
GtkWindow* WindowController::window = nullptr;
GtkButton *WindowController::refreshButton = nullptr;
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
    setAsWallpaperButton = GTK_BUTTON(gtk_builder_get_object(gtk_builder, "set-as-wallpaper-button"));
    spinner = GTK_SPINNER(gtk_builder_get_object(gtk_builder, "spinner"));
    image = GTK_PICTURE(gtk_builder_get_object(gtk_builder, "image"));

    g_signal_connect(refreshButton, "clicked", G_CALLBACK(WindowController::onRefreshButtonClick), spinner);
    g_signal_connect(setAsWallpaperButton, "clicked", G_CALLBACK(WindowController::onSetAsWallpaperButtonClick), spinner);
    g_signal_connect(window, "destroy", G_CALLBACK(WindowController::onWindowClose), nullptr);

    gtk_window_present(window);

    g_object_unref(gtk_builder);
}

void WindowController::onRefreshButtonClick(GtkApplication *app) {
    WindowController::refreshApodImageView();
}

void WindowController::onSetAsWallpaperButtonClick(GtkApplication *app) {
    std::thread([]() {
        backgroundChanger_->changeBackground();
    }).detach();
}

void WindowController::refreshApodImageView() {
    gtk_widget_set_sensitive(GTK_WIDGET(refreshButton), false);
    gtk_spinner_start(spinner);

    std::thread([]() {
        char* imagePath = g_strdup(apodFetcher_->fetchApod().c_str());

        g_main_context_invoke(g_main_context_default(), [](gpointer data) -> gboolean {
            if (quit) {
                g_free(data);
                return G_SOURCE_REMOVE;
            }

            char* p = static_cast<char*>(data);

            int len = std::strlen(p);
            if (len > 0) {
                gtk_picture_set_filename(image, p);
                gtk_widget_set_sensitive(GTK_WIDGET(setAsWallpaperButton), true);
            }
            else {
                std::cerr << "Skipping gtk_picture update: apodFetcher returned an empty string." << std::endl;
            }

            gtk_spinner_stop(spinner);
            gtk_widget_set_sensitive(GTK_WIDGET(refreshButton), true);

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