#pragma once
#include <dpp/dpp.h>
#include "../Command.h"

class SetFaqUrl : public Command
{
private:

    dpp::cluster& bot;

public:

    explicit SetFaqUrl(dpp::cluster& bot_ref) : bot(bot_ref) {}

    std::string getName() const override;
    std::string getDescription() const override;
    void execute(const dpp::slashcommand_t& event) override;
};