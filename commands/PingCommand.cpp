#include "PingCommand.h"
#include <sstream>
#include <iomanip>

std::string PingCommand::getName() const
{
    return "ping";
}
std::string PingCommand::getDescription() const
{
    return "Answers with the Ping";

}

void PingCommand::execute(const dpp::slashcommand_t& event)
{
    double latency_ms = bot.rest_ping * 1000.0;

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << latency_ms;
    std::string formatted_latency = stream.str();

    std::string response = "Pong! 🏓 Latency to the Discord-API: " + formatted_latency + "ms";

    event.reply(response);
}

