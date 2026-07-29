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

enum class ObjectType
{
    Channel,
    Member,
    Role
};

class ModLogModule : public EventModule {
    private:
    BotClient& bot;
    sqlite3* db;

    dpp::snowflake getLogChannelForGuild(dpp::snowflake guild_id);


    void sendLogWithAudit(const dpp::snowflake guild_id, int action_type, const dpp::snowflake& object_id, std::string object_name, std::string title, ObjectType eventObject, LogType logType);

    void sendLog(dpp::snowflake guild_id, LogType type, const std::string& title, const std::string& description);

    std::string objectTypeToString(ObjectType type);

    void onMessageDelete(const dpp::message_delete_t& event);
    void onMemberJoin(const dpp::guild_member_add_t& event);
    void onMemberBan(const dpp::guild_ban_add_t& event);
    void onMemberUnban(const dpp::guild_ban_remove_t& event);
    void onRoleCreated(const dpp::guild_role_create_t& event);
    void onRoleDeleted(const dpp::guild_role_delete_t& event);
    void onChannelCreated(const dpp::channel_create_t& event);
    void onChannelDeleted(const dpp::channel_delete_t& event);
    void onChannelUpdated(const dpp::channel_update_t& event);


    public:

    ModLogModule(BotClient& bot_ref, sqlite3* db_ref)
        : bot(bot_ref), db(db_ref) {}


    void registerHandlers() override;

};