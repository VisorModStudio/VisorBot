#pragma once
#include <string>
 

namespace UrlSafety {
 
    struct ValidationResult {
        bool ok;
        std::string reason; 
    };
 

    ValidationResult validate(const std::string& url);
 
}
 