cmake_minimum_required(VERSION 3.5)
project(apod_wallpaper)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
        GIT_TAG v0.30.0
)

FetchContent_MakeAvailable(httplib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)
include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

add_executable(apod_wallpaper
        src/ApodWallpaper.cpp
        src/ApodFetcher.cpp
        src/ApodFetcher.h
        src/BackgroundChanger.cpp
        src/BackgroundChanger.h
        src/FsUtils.cpp
        src/FsUtils.h
        src/WindowController.cpp
        src/WindowController.h
)

target_include_directories(apod_wallpaper PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(apod_wallpaper PRIVATE httplib::httplib ${GTK4_LIBRARIES})