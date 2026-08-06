#pragma once
#include "BotClient.h"
#include "EventModule.h"
#include <dpp/dpp.h>
#include <string>
#include "sqlite3.h"
#include "AuditLogEvents.h"
#include "GeminiClient.h"

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
    Role,
    Emoji

};

class ModLogModule : public EventModule {
    private:
    BotClient& bot;
    sqlite3* db;
    GeminiClient& gemini;


    dpp::snowflake getLogChannelForGuild(dpp::snowflake guild_id);
    dpp::snowflake getIssueChannelForGuild(dpp::snowflake guild_id);
    dpp::snowflake getIssueResponseChannelForGuild(dpp::snowflake guild_id);
    dpp::snowflake getHelpChannelForGuild(dpp::snowflake guild_id);
    dpp::snowflake getFaqChannelForGuild(dpp::snowflake guild_id);


    void sendLogWithAudit(const dpp::snowflake guild_id, AuditLogEvent action_type, const dpp::snowflake& object_id,
                       std::string title, ObjectType eventObject, LogType logType,
                       std::string precomputed_display = "");

    void sendLog(dpp::snowflake guild_id, LogType type, const std::string& title, const std::string& description);


    void onButtonClick(const dpp::button_click_t& event);

    void onMemberJoin(const dpp::guild_member_add_t& event);
    void onMemberBan(const dpp::guild_ban_add_t& event);
    void onMemberUnban(const dpp::guild_ban_remove_t& event);
    void onRoleCreate(const dpp::guild_role_create_t& event);
    void onRoleDelete(const dpp::guild_role_delete_t& event);
    void onChannelCreate(const dpp::channel_create_t& event);
    void onChannelDelete(const dpp::channel_delete_t& event);
    void onChannelUpdate(const dpp::channel_update_t& event);
    void onMemberRemove(const dpp::guild_member_remove_t& event);
    void onInviteCreate(const dpp::invite_create_t& event);
    void onInviteDelete(const dpp::invite_delete_t& event);
    //TODO Webhook
    void onGuildEmojisUpdate(const dpp::guild_emojis_update_t& event);
    void onMessageDelete(const dpp::message_delete_t& event);
    void onMessageCreate(const dpp::message_create_t& event);
    void onMessageBulkDelete(const dpp::message_delete_bulk_t& event);
    //

    void onThreadCreate(const dpp::thread_create_t& event);

    std::unordered_map<dpp::snowflake, std::map<dpp::snowflake, dpp::emoji>> emoji_cache;
    std::unordered_map<dpp::snowflake, std::pair<dpp::snowflake, std::string>> message_cache;

    public:

    ModLogModule(BotClient& bot_ref, sqlite3* db_ref, GeminiClient& gemini_ref)
        : bot(bot_ref), db(db_ref), gemini(gemini_ref) {}


    void registerHandlers() override;

};