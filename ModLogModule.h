#pragma once
#include "BotClient.h"
#include "EventModule.h"
#include <dpp/dpp.h>
#include <string>
#include "sqlite3.h"

enum class LogType
{
    Info,
    Warning,
    Alert,
    Success
};

class ModLogModule : public EventModule {
    private:
    BotClient& bot;
    sqlite3* db;

    dpp::snowflake getLogChannelForGuild(dpp::snowflake guild_id);

    void sendLog(dpp::snowflake guild_id, LogType type, const std::string& title, const std::string& description);


    void onMessageDelete(const dpp::message_delete_t& event);
    void onMemberJoin(const dpp::guild_member_add_t& event);
    void onMemberBan(const dpp::guild_ban_add_t& event);
    void onMemberUnban(const dpp::guild_ban_remove_t& event);
    void onRoleCreated(const dpp::guild_role_create_t& event);
    void onRoleDeleted(const dpp::guild_role_delete_t& event);



    public:

    ModLogModule(BotClient& bot_ref, sqlite3* db_ref)
        : bot(bot_ref), db(db_ref) {}


    void registerHandlers() override;

};