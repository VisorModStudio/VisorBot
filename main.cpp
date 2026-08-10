#include <dpp/dpp.h>
#include <iostream>
#include <memory>
#include <map>
#include "BotClient.h"
#include "Command.h"
#include "ModLogModule.h"
#include "commands/PingCommand.h"
#include "commands/SetUserPermission.h"
#include "commands/SetDbValue.h"
#include "GeminiClient.h"


int main()
{

    BotClient bot;


    const char* gemini_key_env = std::getenv("GEMINI_API_KEY");

    std::string gemini_key = gemini_key_env ? gemini_key_env : "";

    GeminiClient gemini(bot, gemini_key);

    ModLogModule modLog(bot, bot.getDB(), gemini);
    modLog.registerHandlers();

    std::map<std::string, std::unique_ptr<Command>> commands;


    auto ping = std::make_unique<PingCommand>(bot);
    std::string ping_name = ping->getName();
    commands[ping_name] = std::move(ping);

    auto SetUserPerm = std::make_unique<SetUserPermission>(bot);
    std::string SetUserPerm_name = SetUserPerm->getName();
    commands[SetUserPerm_name] = std::move(SetUserPerm);

    auto set_channel = std::make_unique<SetDbValue>(bot);
    std::string set_channel_name = set_channel->getName();
    commands[set_channel_name] = std::move(set_channel);


    bot.on_slashcommand([&commands, &bot](const dpp::slashcommand_t& event)
    {
        std::string cmd_name = event.command.get_command_name();

        auto it = commands.find(cmd_name);

        if (it == commands.end())
        {
            event.reply(dpp::message("Unknown command: " + cmd_name).set_flags( dpp::m_ephemeral));
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


            if (cmd->getName() == "setuserpermission")
            {
                discord_cmd.add_option(
                    dpp::command_option(dpp::co_user, "user", "The User which permissions should be changed", true )
                );
                discord_cmd.add_option(
                    dpp::command_option(dpp::co_boolean, "botpermissions", "If the user should have bot permissions", true)
                );
            }

            if (cmd->getName() == "setdbvalue")
            {

                discord_cmd.add_option(

                    dpp::command_option(dpp::co_string, "entrytype", "The entry type to be changed", true)
                        .add_choice(dpp::command_option_choice("ModChannel", "Mod_Channel"))
                        .add_choice(dpp::command_option_choice("HelpChannel", "Help_Channel"))
                        .add_choice(dpp::command_option_choice("IssueChannel", "Issue_Channel"))
                        .add_choice(dpp::command_option_choice("FaqChannel", "Faq_Channel"))
                        .add_choice(dpp::command_option_choice("IssueResponseChannel", "IssueRspns_Channel"))
                        .add_choice(dpp::command_option_choice("FaqUrl", "Faq_Url"))
                );


                discord_cmd.add_option(
                    dpp::command_option(dpp::co_channel, "channel_value", "What the entytype should be set to", false)
                        .add_channel_type(dpp::CHANNEL_TEXT)
                        .add_channel_type(dpp::CHANNEL_FORUM)

                    ).add_option(
                        dpp::command_option(dpp::co_string, "text_value", "Or put in a text value(used for setting Url's)", false)
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
