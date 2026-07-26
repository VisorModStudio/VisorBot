#pragma once
#include "../Command.h"



class BulkMessageDelete : public Command
{
    private:

        dpp::cluster& bot;


    public:

        explicit BulkMessageDelete(dpp::cluster& bot_ref) : bot(bot_ref) {}

        std::string getName() const override;
        std::string getDescription() const override;
        void execute(const dpp::slashcommand_t& event) override;
        bool IsAdminCommand = true;
};



