#pragma once
#include "../Command.h"


class PingCommand : public Command
{
public:

    explicit PingCommand(dpp::cluster& bot_ref) : Command(bot_ref) {}

    std::string getName() const override;
    std::string getDescription() const override;
    void execute(const dpp::slashcommand_t& event) override;



};