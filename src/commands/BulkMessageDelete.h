#pragma once
#include "../src/Command.h"



class BulkMessageDelete : public Command
{
public:

    explicit BulkMessageDelete(dpp::cluster& bot_ref) : Command(bot_ref) {}

    std::string getName() const override;
    std::string getDescription() const override;
    void execute(const dpp::slashcommand_t& event) override;
    bool IsAdminCommand = true;
};