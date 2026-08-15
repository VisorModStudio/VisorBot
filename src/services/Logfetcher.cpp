#include <dpp/dpp.h>
#include <string>
#include <sstream>
#include <regex>
#include <functional>

namespace {
    constexpr size_t MAX_LOG_BYTES = 25 * 1024 * 1024;
}

struct LogFetchResult {
    bool success;
    std::string content;
    std::string error;
};

std::string filterLogContent(const std::string& rawLog) {
    static const std::regex prefixRegex(R"(^\[\d{2}:\d{2}:\d{2}\]\s*\[[^\]]+\]\s*\[[^\]]+\]:\s*)");
    static const std::regex errorRegex(R"(ERROR|FATAL|Exception|Cause|\bat\s+[a-zA-Z0-9_.]+\()");

    std::stringstream inputStream(rawLog);
    std::stringstream outputStream;
    std::string line;

    while (std::getline(inputStream, line)) {
        std::string cleanLine = std::regex_replace(line, prefixRegex, "");
        if (std::regex_search(cleanLine, errorRegex)) {
            outputStream << cleanLine << "\n";
        }
    }

    return outputStream.str();
}

void fetchAndCleanLog(dpp::cluster& bot, const std::string& downloadUrl, std::function<void(LogFetchResult)> callback) {
    bot.request(downloadUrl, dpp::m_get, [callback](const dpp::http_request_completion_t& response) {
        if (response.status != 200) {
            callback({false, "", "Download failed with HTTP " + std::to_string(response.status)});
            return;
        }

        if (response.body.size() > MAX_LOG_BYTES) {
            callback({false, "", "Log too large to process."});
            return;
        }

        callback({true, filterLogContent(response.body), ""});
    });
}