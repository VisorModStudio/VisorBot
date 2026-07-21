#include <dpp/dpp.h>
#include "../Command.h"

class SetModChannel : public Command
{
    private:

        dpp::cluster& bot;

    public:

        explicit SetModChannel(dpp::cluster& bot_ref) : bot(bot_ref) {}

        std::string getName() const override;
        std::string getDescription() const override;
        void execute(const dpp::slashcommand_t& event) override;
};


