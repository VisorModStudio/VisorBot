#include "ModLogModule.h"
#include <string>


dpp::snowflake ModLogModule::getLogChannelForGuild(dpp::snowflake guild_id)
{
    dpp::snowflake channel_id = 0;
    const char* sql = "SELECT ModChannelID FROM ServerConfig WHERE GuildID = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int64(stmt, 1, static_cast<int64_t>(guild_id));

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            channel_id = static_cast<dpp::snowflake>(sqlite3_column_int64(stmt, 0));
        }
    }
    sqlite3_finalize(stmt);

    return channel_id;
}


void ModLogModule::registerHandlers() {
    bot.on_guild_ban_add([this](const dpp::guild_ban_add_t& event)
    {
        onMemberBan(event);
    });

    bot.on_guild_role_create([this](const dpp::guild_role_create_t& event) {
        onRoleCreated(event);
    });

    bot.on_guild_role_delete([this](const dpp::guild_role_delete_t& event) {
        onRoleDeleted(event);
    });

    bot.on_message_delete([this](const dpp::message_delete_t& event) {
        onMessageDelete(event);
    });

    bot.on_channel_create([this](const dpp::channel_create_t& event)
    {
        onChannelCreated(event);
    });

    bot.on_channel_delete([this](const dpp::channel_delete_t& event)
    {
        onChannelDeleted(event);
    });

    bot.on_channel_update([this](const dpp::channel_update_t& event)
    {
        onChannelUpdated(event);
    });





    /*
    bot.on_guild_member_add([this](const dpp::guild_member_add_t& event) {
        onMemberJoin(event);

        //TODO
    });
    */
}
std::string objectTypeToString(ObjectType type)
{
    switch (type)
    {
        case ObjectType::Channel: return "Channel";
        case ObjectType::Member: return "Member";
        case ObjectType::Role: return "Role";
    }
};

void ModLogModule::sendLog(dpp::snowflake guild_id, LogType type, const std::string& title, const std::string& description) {
    dpp::snowflake channel_id = getLogChannelForGuild(guild_id);

    if (channel_id == 0) return;

    dpp::embed embed = dpp::embed()
        .set_title(title)
        .set_description(description)
        .set_timestamp(time(nullptr));
    switch (type)
    {
        case LogType::Info: embed.set_color(dpp::colors::light_blue); break;
        case LogType::Warning: embed.set_color(dpp::colors::orange); break;
        case LogType::Alert: embed.set_color(dpp::colors::red); break;
        case LogType::Success: embed.set_color(dpp::colors::green); break;

    }

    bot.message_create(dpp::message(channel_id, embed));
}

void ModLogModule::sendLogWithAudit(const dpp::snowflake guild_id, int action_type, const dpp::snowflake& object_id, std::string object_name, std::string title, ObjectType eventObject, LogType logType)
{
    bot.guild_auditlog_get(guild_id, 0, action_type, 0, 0, 1,
        [this, guild_id, object_id, object_name, title, eventObject, logType](const dpp::confirmation_callback_t& callback) mutable
        {
            if (callback.is_error())
            {
                std::cerr << "Audit log fetch failed: " << callback.get_error().message << std::endl;
                return;
            }

            dpp::auditlog audit = std::get<dpp::auditlog>(callback.value);
            std::string action_by = "No staff";
            std::string object_id_str = object_id.str();

            for (const auto& entry : audit.entries)
            {
                if (entry.target_id == object_id)
                {
                    action_by = (entry.user_id == 0) ? "No staff" : entry.user_id.str();
                    break;
                }
            }

            if (eventObject == ObjectType::Channel)
            {
                object_id_str = "<#" + object_id_str + ">";
            }
            else if (eventObject == ObjectType::Member)
            {
                object_id_str = "@" + object_id_str;
            }
            else if (eventObject == ObjectType::Role)
            {
                object_id_str = "<@&" + object_id_str + ">";
            }

            std::string type_str = objectTypeToString(eventObject);
            std::string description = type_str + ": "+ object_id_str + "\n" + " By: <@" + action_by + ">" ;
            sendLog(guild_id, logType, title, description);
        }
    );
}

void ModLogModule::onMessageDelete(const dpp::message_delete_t& event)
{
    dpp::snowflake guild_id = event.guild_id;
    std::string details = "Channel: <#" + std::to_string(event.channel_id) + ">";

    //TODO
}

void ModLogModule::onMemberBan(const dpp::guild_ban_add_t& event)
{
    dpp::snowflake guild_id = event.banning_guild.id;
    dpp::snowflake banned_user_id = event.banned.id;
    std::string banned_username = event.banned.username;

    sendLogWithAudit(guild_id, 22, banned_user_id, banned_username, "Banned User: ", ObjectType::Member, LogType::Alert);
}

void ModLogModule::onMemberUnban(const dpp::guild_ban_remove_t& event)
{
    dpp::snowflake guild_id = event.unbanning_guild.id;
    dpp::snowflake unbanned_user_id = event.unbanned.id;
    std::string unbanned_username = event.unbanned.username;

    sendLogWithAudit(guild_id, 23, unbanned_user_id, unbanned_username, "Unbanned User: ", ObjectType::Member, LogType::Warning);
}

void::ModLogModule::onRoleCreated(const dpp::guild_role_create_t& event)
{
    dpp::snowflake guild_id = event.creating_guild.id;
    dpp::snowflake role_id = event.created.id;
    std::string role_name = event.created.name;

    sendLogWithAudit(guild_id, 30, role_id, role_name, "Role Created:", ObjectType::Role, LogType::Success);
}

void ModLogModule::onRoleDeleted(const dpp::guild_role_delete_t& event)
{
    dpp::snowflake guild_id = event.deleting_guild.id;
    std::string role_name = event.deleted.name;
    dpp::snowflake role_id = event.deleted.id;

    sendLogWithAudit(guild_id ,30, role_id, role_name, "Role Deleted: ", ObjectType::Role, LogType::Warning);
}

void ModLogModule::onChannelCreated(const dpp::channel_create_t& event)
{
    dpp::snowflake guild_id = event.creating_guild.id;
    std::string channel_name = event.created.name;
    dpp::snowflake channel_id = event.created.id;

    sendLogWithAudit(guild_id, 10, channel_id, channel_name, "Channel Created: ", ObjectType::Channel, LogType::Success);
}

void ModLogModule::onChannelDeleted(const dpp::channel_delete_t& event)
{
    dpp::snowflake guild_id = event.deleting_guild.id;
    std::string channel_name = event.deleted.name;
    dpp::snowflake channel_id = event.deleted.id;

    sendLogWithAudit(guild_id, 12, channel_id, channel_name, "Channel Deleted: ", ObjectType::Channel, LogType::Warning);
}

void ModLogModule::onChannelUpdated(const dpp::channel_update_t& event)
{
    dpp::snowflake guild_id = event.updating_guild.id;
    std::string channel_name = event.updated.name;
    dpp::snowflake channel_id = event.updated.id;

    sendLogWithAudit(guild_id, 11, channel_id, channel_name, "Channel Updated: ", ObjectType::Channel, LogType::Success);
}



