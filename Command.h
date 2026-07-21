#pragma once
#include <dpp/dpp.h>
#include <string>


class Command
{
    public:
    virtual ~Command() = default ;

    virtual std::string getName() const = 0;

    virtual std::string getDescription() const = 0;

    virtual void execute(const dpp::slashcommand_t& event) = 0;

};