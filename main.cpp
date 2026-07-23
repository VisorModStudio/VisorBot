#include <dpp/dpp.h>
#include <iostream>
#include <memory>
#include <map>
#include "BotClient.h"
#include "Command.h"
#include "commands/PingCommand.h"
#include "commands/SetModChannel.h"

int main()
{

    BotClient bot;

    std::map<std::string, std::unique_ptr<Command>> commands;


    auto ping = std::make_unique<PingCommand>(bot);
    std::string ping_name = ping->getName();
    commands[ping_name] = std::move(ping);


    auto set_mod = std::make_unique<SetModChannel>(bot);
    std::string set_mod_name = set_mod->getName();
    commands[set_mod_name] = std::move(set_mod);

    bot.on_slashcommand([&commands](const dpp::slashcommand_t& event)
    {
        std::string cmd_name = event.command.get_command_name();

        auto it = commands.find(cmd_name);
        if (it != commands.end())
        {
            it->second->execute(event);
        }
        else
        {
            event.reply("Unknown command: " + cmd_name);
        }
    });

    bot.on_ready([&bot, &commands](const dpp::ready_t& event)
{
    if (dpp::run_once<struct register_bot_commands>())
    {
        for (const auto& [name, cmd] : commands)
        {
            dpp::slashcommand discord_cmd(cmd->getName(), cmd->getDescription(), bot.me.id);


            if (cmd->getName() == "setmodchannel")
            {
                discord_cmd.add_option(
                    dpp::command_option(dpp::co_channel, "channel", "The channel where admin logs should be sent", true)
                        .add_channel_type(dpp::CHANNEL_TEXT)
                );
            }
            if (cmd->getName() == "SetUserPermission")
            {
                discord_cmd.add_option(
                    dpp::command_option(dpp::co_user, "User", "The User which permissions should be changed", true )
                );
                discord_cmd.add_option(
                    dpp::command_option(dpp::co_boolean, "BotPermissions", "If the user should have bot permissions", true)
                );
            }

            bot.global_command_create(discord_cmd);
        }
    }

    std::cout << "Visor bot online and commands are registered!" << std::endl;
});


    bot.start(dpp::st_wait);

    return 0;
}