#include <httplib.h>

#include "ApodFetcher.h"

#include <filesystem>
#include <fstream>

ApodFetcher::ApodFetcher(const std::string &url) : httpClient(url) {
    std::cout << "Starting HTTP client with URL " + url << std::endl;
    cacheDir = std::string(std::getenv("HOME")) + "/.cache/apod-fetcher";
    std::filesystem::create_directories(cacheDir);

}

std::string ApodFetcher::fetchApod() {
    return fetchApodImage(fetchApodImagePath());
}

std::string ApodFetcher::fetchApodImagePath() {
    std::string path;
    std::string body;
    std::string extension;

    if (auto res = httpClient.Get("/apod/")) {
        body = res->body;
    }
    else {
        std::cerr << "Error fetching APOD:" << std::endl;
        std::cerr << res.error() << std::endl;

        // todo: error handling
        return "";
    }

    std::regex apodImagePathRegex(R"((<a href="image\/[0-9]{4}\/).+\.*.+(">))",
        std::regex_constants::ECMAScript | std::regex_constants::icase);

    auto match = std::sregex_iterator(body.begin(), body.end(), apodImagePathRegex);

    auto matchCount = std::distance(match, std::sregex_iterator());

    if (matchCount == 1) {
        std::string matchStr = match->str();

        auto firstQuotePos = matchStr.find_first_of('\"');
        auto lastQuotePos = matchStr.find_last_of('\"');

        path = "/" + matchStr.substr(firstQuotePos + 1, lastQuotePos - 1 - firstQuotePos);
    }
    else {
        std::cerr << "Error fetching APOD:" << std::endl;
        std::cerr << "Found " << matchCount << " regex matches (expected 1)";

        // todo: error handling
        return "";
    }

    return path;
}

std::string ApodFetcher::fetchApodImage(const std::string &remotePath) {
    std::string body;
    std::string extension;
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
            }
        }

        savePath = cacheDir + "/apod." + extension;
        std::cout << "Saving to " + savePath << std::endl;
        std::ofstream apodImageOfstream;
        apodImageOfstream.open(cacheDir + "/apod." + extension);
        apodImageOfstream << body;
        apodImageOfstream.close();
    }
    else {
        std::cerr << "Error fetching APOD image file:" << std::endl;
        std::cerr << res.error() << std::endl;

        // todo: error handling
        return "";
    }

    return savePath;
}
