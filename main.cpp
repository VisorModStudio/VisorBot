#include <dpp/dpp.h>
#include <iostream>
#include <memory>
#include <map>
#include "BotClient.h"
#include "Command.h"
#include "ModLogModule.h"
#include "commands/PingCommand.h"
#include "commands/SetModChannel.h"
#include "commands/SetUserPermission.h"

int main()
{

    BotClient bot;

    ModLogModule modLog(bot, bot.getDB());
    modLog.registerHandlers();

    std::map<std::string, std::unique_ptr<Command>> commands;


    auto ping = std::make_unique<PingCommand>(bot);
    std::string ping_name = ping->getName();
    commands[ping_name] = std::move(ping);


    auto set_mod = std::make_unique<SetModChannel>(bot);
    std::string set_mod_name = set_mod->getName();
    commands[set_mod_name] = std::move(set_mod);
    auto SetUserPerm = std::make_unique<SetUserPermission>(bot);
    std::string SetUserPerm_name = SetUserPerm->getName();
    commands[SetUserPerm_name] = std::move(SetUserPerm);

    bot.on_slashcommand([&commands, &bot](const dpp::slashcommand_t& event)
    {
        std::string cmd_name = event.command.get_command_name();

        auto it = commands.find(cmd_name);

        if (it == commands.end())
        {
            event.reply("Unknown command: " + cmd_name);
            return;
        }
        if (it->second->requiresAdmin())
        {
            dpp::snowflake user_id = event.command.get_issuing_user().id;
            dpp::permission userDiscordPermission = event.command.get_resolved_permission(user_id);
            if (!bot.userHasAdminPermission(user_id) && !userDiscordPermission.can(dpp::p_administrator))
            {
                event.reply(dpp::message("You don´t have permission to use this command").set_flags( dpp::m_ephemeral));
                return;
            }
        }

        it->second->execute(event);
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
            if (cmd->getName() == "setuserpermission")
            {
                discord_cmd.add_option(
                    dpp::command_option(dpp::co_user, "user", "The User which permissions should be changed", true )
                );
                discord_cmd.add_option(
                    dpp::command_option(dpp::co_boolean, "botpermissions", "If the user should have bot permissions", true)
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
