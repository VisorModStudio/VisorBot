#pragma once
#include "BotClient.h"
#include "dpp/dpp.h"

class GameLogHelper
{
    public:

    explicit GameLogHelper(BotClient& bot_ref) : bot(bot_ref) {}

    dpp::message getLogHelpMsg(dpp::snowflake channelID);
    void sendLogHelpMsg(const dpp::message& msg);

    private:
    BotClient& bot;
};


