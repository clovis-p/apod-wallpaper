#include <httplib.h>

#include "ApodFetcher.h"

#include <chrono>
#include <filesystem>
#include <fstream>

#include "FsUtils.h"

ApodFetcher::ApodFetcher(const std::string &url) : httpClient(url) {
    // todo: what is that doing here
    std::cout << "Starting HTTP client with URL " + url << std::endl;
}

/**
 * Fetches today's APOD and downloads it
 * @return Path in local filesystem to downloaded APOD file on success, empty string on failure
 */
std::string ApodFetcher::fetchApod() {
    const auto now = std::chrono::system_clock::now();
    const auto nowYmd = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(now));

    return fetchApod(nowYmd);
}

/**
 * Checks if APOD exists on disk and downloads it if it's missing.
 * @param date The date of the requested APOD
 * @return Path in local filesystem to APOD file on success, empty string on failure
 */
std::string ApodFetcher::fetchApod(std::chrono::year_month_day date) {
    const std::string wallpaperDir = FsUtils::getCacheDir() + "/wallpaper";
    std::string saveDir = wallpaperDir + "/" + dateToWallpaperPath(date);
    std::string savePath = saveDir +  + "/apod.";

    if (std::filesystem::exists(savePath + "jpg")) {
        return savePath + "jpg";
    }
    if (std::filesystem::exists(savePath + "png")) {
        return savePath + "png";
    }
    if (std::filesystem::exists(savePath + "gif")) {
        return savePath + "gif";
    }
    if (std::filesystem::exists(savePath + "webp")) {
        return savePath + "webp";
    }

    std::cout << "APOD " << dateToFormattedString(date) << " was not found in cache" << std::endl;

    std::string remoteApodImagePath = fetchApodImagePath(date);

    if (remoteApodImagePath.empty()) {
        return "";
    }

    return fetchApodImage(remoteApodImagePath, date);
}

/**
 * @return Remote path to today's APOD file on success, empty string on failure
 */
std::string ApodFetcher::fetchApodImagePath(std::chrono::year_month_day date) {
    std::string remoteImagePath;
    std::string body;
    std::string extension;

    std::cout << "Fetching today's APOD..." << std::endl;

    std::string remoteApodPagePath = std::string("/apod/ap") + dateToFormattedString(date) + std::string(".html");

    std::cout << "Sending request to " << remoteApodPagePath << std::endl;

    if (auto res = httpClient.Get(remoteApodPagePath)) {
        body = res->body;
    }
    else {
        std::cerr << "Error fetching APOD:" << std::endl;
        std::cerr << res.error() << std::endl;

        return "";
    }

    std::regex apodImagePathRegex(R"((<a href="image\/[0-9]{4}\/)[a-zA-Z0-9_\-]+\.[a-zA-Z0-9]{3,4}(">))",
        std::regex_constants::ECMAScript | std::regex_constants::icase);

    auto match = std::sregex_iterator(body.begin(), body.end(), apodImagePathRegex);

    auto matchCount = std::distance(match, std::sregex_iterator());

    if (matchCount == 1) {
        std::string matchStr = match->str();

        auto firstQuotePos = matchStr.find_first_of('\"');
        auto lastQuotePos = matchStr.find_last_of('\"');

        remoteImagePath = "/" + matchStr.substr(firstQuotePos + 1, lastQuotePos - 1 - firstQuotePos);
    }
    else {
        std::cerr << "Error fetching APOD:" << std::endl;
        std::cerr << "Found " << matchCount << " regex matches (expected 1)";

        return "";
    }

    return remoteImagePath;
}

/**
 * @param remotePath Remote path to today's APOD file
 * @return Path in local filesystem to downloaded APOD file on success, empty string on failure
 */
std::string ApodFetcher::fetchApodImage(const std::string &remotePath, std::chrono::year_month_day date) {
    std::string body;
    std::string extension;
    std::string saveDir;
    std::string savePath;

    if (auto res = httpClient.Get(remotePath)) {
        body = res->body;

        if (res->headers.contains("Content-Type")) {
            auto contentTypeEntry = res->headers.find("Content-Type");
            std::string contentType = contentTypeEntry->second;

            if (contentType == "image/png") {
                extension = "png";
            }
            else if (contentType == "image/jpeg") {
                extension = "jpg";
            }
            else if (contentType == "image/gif") {
                extension = "gif";
            }
            else if (contentType == "image/webp") {
                extension = "webp";
            }
            else {
                std::cout << "Unsupported type: " + contentType + ". No wallpaper today!" << std::endl;

                return "";
            }
        }

        const std::string wallpaperDir = FsUtils::getCacheDir() + "/wallpaper";

        saveDir = wallpaperDir + "/" + dateToWallpaperPath(date);
        savePath = saveDir +  + "/apod." + extension;
        std::filesystem::create_directories(saveDir);
        std::cout << "Saving to " + savePath << std::endl;
        std::ofstream apodImageOfstream;
        apodImageOfstream.open(savePath);
        apodImageOfstream << body;
        apodImageOfstream.close();
    }
    else {
        std::cerr << "Error fetching APOD image file:" << std::endl;
        std::cerr << res.error() << std::endl;

        return "";
    }

    return savePath;
}

/**
 * Turns a std::chrono::year_month_day into a string in the format YYMMDD
 * @param date The date to format
 * @return The formatted string
 */
std::string ApodFetcher::dateToFormattedString(std::chrono::year_month_day date) {
    int year = static_cast<int>(date.year());
    unsigned month = static_cast<unsigned>(date.month());
    unsigned day = static_cast<unsigned>(date.day());

    std::string formattedString = std::format("{:02}{:02}{:02}", year % 100, month, day);
    return formattedString;
}

/**
 * Turns a std::chrono::year_month_day into the path to the directory containing that day's wallpaper.
 * @param date The requested date
 * @return The path to the directory
 */
std::string ApodFetcher::dateToWallpaperPath(std::chrono::year_month_day date) {
    int year = static_cast<int>(date.year());
    unsigned month = static_cast<unsigned>(date.month());
    unsigned day = static_cast<unsigned>(date.day());

    std::string formattedString = std::format("{:02}/{:02}/{:02}", year % 100, month, day);
    return formattedString;
}
