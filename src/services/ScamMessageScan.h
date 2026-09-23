#pragma once

#include <vector>
#include <unordered_map>
#include <dpp/dpp.h>

struct ImagePost {
    dpp::snowflake msgID;
    dpp::snowflake channelID;
    int imageCount;
    time_t timestamp;
};

class ScamMessageScan

{
public:
    std::string ScanMessage(dpp::cluster& bot, dpp::snowflake msgID, dpp::snowflake userID, dpp::snowflake channelID, int imageCount,
        time_t timestamp, std::vector<dpp::attachment> attachments);



private:

    std::unordered_map<dpp::snowflake, std::vector<ImagePost>> userPosts;



};


