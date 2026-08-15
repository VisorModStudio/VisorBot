#pragma once
#include <dpp/dpp.h>
#include "../src/Command.h"

class SetServerConfigValue : public Command
{
public:

    explicit SetServerConfigValue(dpp::cluster& bot_ref) : Command(bot_ref) {}

    std::string getName() const override;
    std::string getDescription() const override;
    void execute(const dpp::slashcommand_t& event) override;
};