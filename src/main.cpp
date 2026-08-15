#include <dpp/dpp.h>
#include <iostream>
#include <memory>
#include <map>
#include "../BotClient.h"
#include "../Command.h"
#include "../services/ModLogModule.h"
#include "../commands/PingCommand.h"
#include "../commands/SetServerConfigValue.h"
#include "../commands/SetKnowledgeSource.h"
#include "../services/GeminiClient.h"
#include "../services/KnowledgeCache.h"


int main()
{

    BotClient bot;


    const char* gemini_key_env = std::getenv("GEMINI_API_KEY");

    std::string gemini_key = gemini_key_env ? gemini_key_env : "";

    GeminiClient gemini(bot, gemini_key);

    ModLogModule modLog(bot, bot.getDB(), gemini, bot.knowledgeCache);
    modLog.registerHandlers();


    std::map<std::string, std::unique_ptr<Command>> commands;


    auto ping = std::make_unique<PingCommand>(bot);
    std::string ping_name = ping->getName();
    commands[ping_name] = std::move(ping);

    auto set_channel = std::make_unique<SetServerConfigValue>(bot);
    std::string set_channel_name = set_channel->getName();
    commands[set_channel_name] = std::move(set_channel);

    auto set_knowledge_source = std::make_unique<SetKnowledgeSource>(bot, bot.knowledgeCache);
    std::string set_knowledge_source_name = set_knowledge_source->getName();
    commands[set_knowledge_source_name] = std::move(set_knowledge_source);


    bot.on_slashcommand([&commands, &bot, modLog](const dpp::slashcommand_t& event)
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
            dpp::snowflake guild_id = event.command.get_guild().id;
            std::string my_id = "1435661653176746155";
            dpp::permission userDiscordPermission = event.command.get_resolved_permission(user_id);
            dpp::snowflake BotAccessRole = modLog.getColumnFromServerConfig(guild_id, "BotAccessRole");
            const std::vector<dpp::snowflake>& member_roles = event.command.member.get_roles();

            bool has_role = std::find(member_roles.begin(), member_roles.end(), BotAccessRole) != member_roles.end();

            if (!has_role && user_id.str() != my_id && !bot.userHasAdminPermission(user_id))
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
        std::vector<dpp::slashcommand> discord_commands;

        for (const auto& [name, cmd] : commands)
        {
            dpp::slashcommand discord_cmd(cmd->getName(), cmd->getDescription(), bot.me.id);


            if (cmd->getName() == "setdbvalue")
            {

                discord_cmd.add_option(

                    dpp::command_option(dpp::co_string, "entrytype", "The entry type to be changed", true)
                        .add_choice(dpp::command_option_choice("ModChannel", "Mod_Channel"))
                        .add_choice(dpp::command_option_choice("HelpChannel", "Help_Channel"))
                        .add_choice(dpp::command_option_choice("IssueChannel", "Issue_Channel"))
                        .add_choice(dpp::command_option_choice("FaqChannel", "Faq_Channel"))
                        .add_choice(dpp::command_option_choice("IssueResponseChannel", "IssueRspns_Channel"))
                        .add_choice(dpp::command_option_choice("BotAccessRole", "BotAccess_Role"))

                );


                discord_cmd.add_option(
                    dpp::command_option(dpp::co_channel, "channel_value", "What the entytype should be set to", false)
                        .add_channel_type(dpp::CHANNEL_TEXT)
                        .add_channel_type(dpp::CHANNEL_FORUM)

                    ).add_option(
                        dpp::command_option(dpp::co_string, "text_value", "Or put in a text value(used for setting Url's)", false)

                    ).add_option(
                        dpp::command_option(dpp::co_role, "role_value", "Role which the entry should be set to")

                    );


            }

            if (cmd->getName() == "setknowledgesource")
            {
                discord_cmd.add_option(
                    dpp::command_option(dpp::co_string, "name", "The name of the knowledge source", true)
                );
                discord_cmd.add_option(
                    dpp::command_option(dpp::co_string, "url", "The URL of the knowledge source", true)
                );
            }

            discord_commands.push_back(discord_cmd);
        }


        bot.global_bulk_command_create(discord_commands);

        std::thread([&bot]()
        {
            BotClient& client = static_cast<BotClient&>(bot);
            sqlite3* db = client.getDB();

            std::cout << "Reloading knowledge sources..." << std::endl;

            try
            {
                client.knowledgeCache.reloadAll(db);
                std::cout << "Knowledge Sources Up-To-Date!" << std::endl;
            }
            catch (const std::exception& e)
            {
                std::cerr << "Failed to reload knowledge sources: " << e.what() << std::endl;
            }
            catch (...)
            {
                std::cerr << "Failed to reload knowledge sources: unknown error" << std::endl;
            }
        }).detach();
    }

    std::cout << "Visor bot online and commands are registered!" << std::endl;
});


    bot.start(dpp::st_wait);

    return 0;
}