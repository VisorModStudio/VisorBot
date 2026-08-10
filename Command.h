#pragma once
#include <dpp/dpp.h>
#include <string>

class Command
{
public:
    explicit Command(dpp::cluster& bot_ref) : bot(bot_ref) {}

    virtual ~Command() = default ;

    virtual std::string getName() const = 0;

    virtual std::string getDescription() const = 0;

    virtual void execute(const dpp::slashcommand_t& event) = 0;

    virtual bool requiresAdmin() const {return true;}

protected:
    dpp::cluster& bot;

};