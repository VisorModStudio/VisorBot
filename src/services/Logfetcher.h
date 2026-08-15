#pragma once

#include <dpp/dpp.h>
#include <string>
#include <functional>

struct LogFetchResult {
    bool success;
    std::string content; 
    std::string error;
};

std::string filterLogContent(const std::string& rawLog);

void fetchAndCleanLog(dpp::cluster& bot, const std::string& downloadUrl, std::function<void(LogFetchResult)> callback);