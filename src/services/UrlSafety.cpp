#include "UrlSafety.h"

#include <regex>
#include <algorithm>
#include <array>

namespace {

    const std::array<std::string, 3> ALLOWED_HOSTS = {
        "raw.githubusercontent.com",
        "github.com",
        "gist.githubusercontent.com"
    };

    std::string toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
                        [](unsigned char c) { return std::tolower(c); });
        return s;
    }

    bool parseUrl(const std::string& url, std::string& scheme, std::string& host) {
        static const std::regex re(R"(^(https?)://([^/:?#]+)(?::\d+)?(?:[/?#].*)?$)",
                                    std::regex::icase);
        std::smatch m;
        if (!std::regex_match(url, m, re)) {
            return false;
        }
        scheme = toLower(m[1].str());
        host = toLower(m[2].str());
        return true;
    }

    bool isAllowedHost(const std::string& host) {
        for (const auto& allowed : ALLOWED_HOSTS) {
            if (host == allowed) return true;
            std::string suffix = "." + allowed;
            if (host.size() > suffix.size() &&
                host.compare(host.size() - suffix.size(), suffix.size(), suffix) == 0) {
                return true;
                }
        }
        return false;
    }

}

namespace UrlSafety {

    ValidationResult validate(const std::string& url) {
        std::string scheme, host;
        if (!parseUrl(url, scheme, host)) {
            return {false, "The URL is invalid or malformed."};
        }

        if (scheme != "https") {
            return {false, "Only https:// URLs are allowed."};
        }

        if (!isAllowedHost(host)) {
            return {false, "Only URLs from github.com, raw.githubusercontent.com, or gist.githubusercontent.com are allowed."};
        }

        return {true, ""};
    }

}