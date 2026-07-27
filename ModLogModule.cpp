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



    /*
    bot.on_guild_member_add([this](const dpp::guild_member_add_t& event) {
        onMemberJoin(event);

        //TODO
    });
    */
}

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

    bot.guild_auditlog_get(guild_id, 0, 22, 0, 0, 1,
        [this, guild_id, banned_user_id, banned_username](const dpp::confirmation_callback_t& callback)
        {
            if (callback.is_error())
            {
                std::cerr << "Audit log fetch failed: " << callback.get_error().message << std::endl;
                return;
            }

            dpp::auditlog audit = std::get<dpp::auditlog>(callback.value);
            std::string reason = "No reason";

            for (const auto& entry : audit.entries)
            {
                if (entry.target_id == banned_user_id)
                {
                    reason = entry.reason.empty() ? "No reason" : entry.reason;
                    break;
                }
            }

            std::string title = "Banned: " + banned_username;
            std::string description = "Reason: " + reason;
            sendLog(guild_id, LogType::Alert, title, description);
        }
    );
}

void ModLogModule::onMemberUnban(const dpp::guild_ban_remove_t& event)
{
    dpp::snowflake guild_id = event.unbanning_guild.id;
    dpp::snowflake unbanned_user_id = event.unbanned.id;
    std::string banned_username = event.unbanned.username;
    std::string title = "Unbanned:" + banned_username;
    //std::string description = "Unbanned by:" + event.;




    bot.guild_auditlog_get(guild_id, 0, 23, 0, 0, 1,
        [this, guild_id, unbanned_user_id, banned_username](const dpp::confirmation_callback_t& callback)
        {
            if (callback.is_error())
            {
                std::cerr << "Audit log fetch failed: " << callback.get_error().message << std::endl;
                return;
            }

            dpp::auditlog audit = std::get<dpp::auditlog>(callback.value);
            std::string unbanningStaff = "No staff";

            for (const auto& entry : audit.entries)
            {
                if (entry.target_id == unbanned_user_id)
                {
                    unbanningStaff = (entry.user_id == 0) ? "No staff" : entry.user_id.str();
                    break;
                }
            }

            std::string title = "Unbanned: " + banned_username;
            std::string description = "By: " + unbanningStaff;
            sendLog(guild_id, LogType::Alert, title, description);
        }
    );
}

void ModLogModule::onRoleCreated(const dpp::guild_role_create_t& event)
{
    dpp::snowflake guild_id = event.creating_guild.id;
    std::string role_name = event.created.name;
    dpp::snowflake role_id = event.created.id;


    bot.guild_auditlog_get(guild_id, 0, 30, 0, 0, 1,
        [this, guild_id, role_id, role_name](const dpp::confirmation_callback_t& callback)
        {
            if (callback.is_error())
            {
                std::cerr << "Audit log fetch failed: " << callback.get_error().message << std::endl;
                return;
            }

            dpp::auditlog audit = std::get<dpp::auditlog>(callback.value);
            std::string created_by = "No staff";

            for (const auto& entry : audit.entries)
            {
                if (entry.target_id == role_id)
                {
                    created_by = (entry.user_id == 0) ? "No staff" : entry.user_id.str();
                    break;
                }
            }

            std::string title = "Role Created:";
            std::string description = "Role: <@&" + role_id.str() +">\n" + " By: <@" + created_by + ">" ;
            sendLog(guild_id, LogType::Success, title, description);
        }
    );
}

void ModLogModule::onRoleDeleted(const dpp::guild_role_delete_t& event)
{
    dpp::snowflake guild_id = event.deleting_guild.id;
    std::string role_name = event.deleted.name;
    dpp::snowflake role_id = event.deleted.id;


    bot.guild_auditlog_get(guild_id, 0, 30, 0, 0, 1,
        [this, guild_id, role_id, role_name](const dpp::confirmation_callback_t& callback)
        {
            if (callback.is_error())
            {
                std::cerr << "Audit log fetch failed: " << callback.get_error().message << std::endl;
                return;
            }

            dpp::auditlog audit = std::get<dpp::auditlog>(callback.value);
            std::string deleted_by = "No staff";

            for (const auto& entry : audit.entries)
            {
                if (entry.target_id == role_id)
                {
                    deleted_by = (entry.user_id == 0) ? "No staff" : entry.user_id.str();
                    break;
                }
            }

            std::string title = "Role Deleted:";
            std::string description = "Role: " + role_name +"\n" + " By: <@" + deleted_by + ">" ;
            sendLog(guild_id, LogType::Warning, title, description);
        }
    );
}



