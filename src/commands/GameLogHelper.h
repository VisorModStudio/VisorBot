#pragma once
#include "Command.h"

class GameLogHelperCommand : public  Command
{
public:


    explicit GameLogHelperCommand(dpp::cluster& bot_ref) : Command(bot_ref) {}

    std::string getName() const override;
    std::string getDescription() const override;
    void execute(const dpp::slashcommand_t& event) override;
};


