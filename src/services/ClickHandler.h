#pragma once
#include "dpp/dpp.h"
#include "../src/BotClient.h"
#include "../services/GameLogHelper.h"

class ClickHandler
{
public:
    explicit ClickHandler(BotClient& bot_ref) : bot(bot_ref), gameLogHelper(bot_ref)
    {
    }

    void onSelectClick(const dpp::select_click_t& event);
    void onButtonClick(const dpp::button_click_t& event);

    private:
    BotClient& bot;
    GameLogHelper gameLogHelper;
};



