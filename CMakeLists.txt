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

add_executable(apod_wallpaper
        src/ApodWallpaper.cpp
        src/ApodFetcher.cpp
        src/ApodFetcher.h
        src/BackgroundChanger.cpp
        src/BackgroundChanger.h
        src/FsUtils.cpp
        src/FsUtils.h
)

target_include_directories(apod_wallpaper PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(apod_wallpaper PRIVATE httplib::httplib)