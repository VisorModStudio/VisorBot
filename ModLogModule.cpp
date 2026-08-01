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

    bot.on_invite_create([this](const dpp::invite_create_t& event)
    {
        onInviteCreated(event);
    });

    bot.on_invite_delete([this](const dpp::invite_delete_t& event)
    {
        onInviteDeleted(event);
    });

    //TODO Webhook

    bot.on_guild_emojis_update([this](const dpp::guild_emojis_update_t& event)
    {
        onGuildEmojisUpdate(event);
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
    case ObjectType::Emoji: return "Emoji";
    }
    return "Unknown";
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

void ModLogModule::sendLogWithAudit(const dpp::snowflake guild_id, AuditLogEvent action_type, const dpp::snowflake& object_id,
                                     std::string title, ObjectType eventObject, LogType logType,
                                     std::string precomputed_display)
{
    bot.guild_auditlog_get(guild_id, 0, static_cast<int>(action_type), 0, 0, 1,
        [this, guild_id, object_id, title, eventObject, logType, precomputed_display](const dpp::confirmation_callback_t& callback) mutable
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
                object_id_str = "<@" + object_id_str + ">";
            }
            else if (eventObject == ObjectType::Role)
            {
                object_id_str = "<@&" + object_id_str + ">";
            }
            else if (eventObject == ObjectType::Emoji)
            {
                object_id_str = precomputed_display.empty() ? object_id_str : precomputed_display;
            }

            std::string type_str = objectTypeToString(eventObject);
            std::string description = type_str + ": " + object_id_str + "\n By: <@" + action_by + ">";
            sendLog(guild_id, logType, title, description);
        }
    );
}

void ModLogModule::onChannelCreated(const dpp::channel_create_t& event)
{
    dpp::snowflake guild_id = event.creating_guild.id;
    std::string channel_name = event.created.name;
    dpp::snowflake channel_id = event.created.id;

    sendLogWithAudit(guild_id, AuditLogEvent::ChannelCreate, channel_id, "Channel Created: ", ObjectType::Channel, LogType::Success);
}

void ModLogModule::onChannelUpdated(const dpp::channel_update_t& event)
{
    dpp::snowflake guild_id = event.updating_guild.id;
    std::string channel_name = event.updated.name;
    dpp::snowflake channel_id = event.updated.id;

    sendLogWithAudit(guild_id, AuditLogEvent::ChannelUpdate, channel_id, "Channel Updated: ", ObjectType::Channel, LogType::Success);
}

void ModLogModule::onChannelDeleted(const dpp::channel_delete_t& event)
{
    dpp::snowflake guild_id = event.deleting_guild.id;
    std::string channel_name = event.deleted.name;
    dpp::snowflake channel_id = event.deleted.id;

    sendLogWithAudit(guild_id, AuditLogEvent::ChannelDelete, channel_id, "Channel Deleted: ", ObjectType::Channel, LogType::Warning);
}

void ModLogModule::onMemberRemove(const dpp::guild_member_remove_t& event)
{
    dpp::snowflake guild_id = event.removing_guild.id;
    dpp::snowflake user_id = event.removed.id;
    std::string username = event.removed.username;

    bot.guild_auditlog_get(guild_id, 0, static_cast<int>(AuditLogEvent::MemberKick), 0, 0, 5,
        [this, guild_id, user_id, username](const dpp::confirmation_callback_t& callback)
        {
            if (callback.is_error())
            {
                std::cerr << "Audit log fetch failed: " << callback.get_error().message << std::endl;
                return;
            }

            dpp::auditlog audit = std::get<dpp::auditlog>(callback.value);

            for (const auto& entry : audit.entries)
            {
                if (entry.target_id == user_id)
                {
                    std::string kicked_by = (entry.user_id == 0) ? "No staff" : entry.user_id.str();
                    std::string title = "Member Kicked: ";
                    std::string description = "Member: <@" + user_id.str() + ">\nBy: <@" + kicked_by + ">";
                    sendLog(guild_id, LogType::Alert, title, description);
                    return;
                }
            }
        });
}

void ModLogModule::onMemberBan(const dpp::guild_ban_add_t& event)
{
    dpp::snowflake guild_id = event.banning_guild.id;
    dpp::snowflake banned_user_id = event.banned.id;
    std::string banned_username = event.banned.username;

    sendLogWithAudit(guild_id, AuditLogEvent::MemberBanAdd, banned_user_id, "Banned User: ", ObjectType::Member, LogType::Alert);
}


void ModLogModule::onMemberUnban(const dpp::guild_ban_remove_t& event)
{
    dpp::snowflake guild_id = event.unbanning_guild.id;
    dpp::snowflake unbanned_user_id = event.unbanned.id;
    std::string unbanned_username = event.unbanned.username;

    sendLogWithAudit(guild_id, AuditLogEvent::MemberBanRemove, unbanned_user_id, "Unbanned User: ", ObjectType::Member, LogType::Warning);
}

void ModLogModule::onRoleCreated(const dpp::guild_role_create_t& event)
{
    dpp::snowflake guild_id = event.creating_guild.id;
    dpp::snowflake role_id = event.created.id;
    std::string role_name = event.created.name;

    sendLogWithAudit(guild_id, AuditLogEvent::RoleCreate, role_id, "Role Created:", ObjectType::Role, LogType::Success);
}

void ModLogModule::onRoleDeleted(const dpp::guild_role_delete_t& event)
{
    dpp::snowflake guild_id = event.deleting_guild.id;
    std::string role_name = event.deleted.name;
    dpp::snowflake role_id = event.deleted.id;

    sendLogWithAudit(guild_id ,AuditLogEvent::RoleDelete, role_id, "Role Deleted: ", ObjectType::Role, LogType::Warning);
}

void ModLogModule::onInviteCreated(const dpp::invite_create_t& event)
{
    dpp::snowflake guild_id = event.created_invite.guild_id;
    std::string invite_code = event.created_invite.code;
    dpp::snowflake inviter_id = event.created_invite.inviter_id;

    sendLog(guild_id, LogType::Success, "Invite Created:", "By: <@" + inviter_id.str() + ">\n" + "Invite Code: " + invite_code);
}

void ModLogModule::onInviteDeleted(const dpp::invite_delete_t& event)
{
    dpp::snowflake guild_id = event.deleted_invite.guild_id;
    std::string invite_code = event.deleted_invite.code;
    std::string title = "Invite Deleted: ";

    bot.guild_auditlog_get(guild_id, 0, static_cast<int>(AuditLogEvent::InviteDelete), 0, 0, 1,
        [this, guild_id, invite_code, title](const dpp::confirmation_callback_t& callback) mutable
        {
            if (callback.is_error())
            {
                std::cerr << "Audit log fetch failed: " << callback.get_error().message << std::endl;
                return;
            }

            dpp::auditlog audit = std::get<dpp::auditlog>(callback.value);
            std::string action_by = "No staff";


            for (const auto& entry : audit.entries)
            {
                for (const auto& change : entry.changes)
                {
                    if (change.key == "code")
                    {
                        std::string cleaned_value = change.old_value;

                        if (cleaned_value.size() >= 2 && cleaned_value.front() == '"' && cleaned_value.back() == '"') //fuck discord
                        {
                            cleaned_value = cleaned_value.substr(1, cleaned_value.size() - 2);
                        }

                        if (cleaned_value == invite_code)
                        {
                            action_by = (entry.user_id == 0) ? "No staff" : entry.user_id.str();
                            break;
                        }
                    }
                }
            }



            std::string description =  " By: <@" + action_by + ">" "\n" + "Invite Code: "+ invite_code;
            sendLog(guild_id, LogType::Warning, title, description);
        }
    );
}

//TODO Webhook


void ModLogModule::onGuildEmojisUpdate(const dpp::guild_emojis_update_t& event)
{
    dpp::snowflake guild_id = event.updating_guild.id;

    std::set<dpp::snowflake> new_ids;
    for (const auto& emoji_id : event.emojis)
    {
        new_ids.insert(emoji_id);
    }

    auto it = emoji_cache.find(guild_id);
    if (it == emoji_cache.end())
    {
        bot.guild_emojis_get(guild_id, [this, guild_id](const dpp::confirmation_callback_t& callback)
        {
            if (callback.is_error()) return;

            std::map<dpp::snowflake, dpp::emoji> baseline;
            dpp::emoji_map emojis = std::get<dpp::emoji_map>(callback.value);
            for (const auto& [emoji_id, emoji] : emojis)
            {
                baseline[emoji_id] = emoji;
            }
            emoji_cache[guild_id] = baseline;
        });
        return;
    }

    std::map<dpp::snowflake, dpp::emoji>& cached = it->second;


    for (const auto& emoji_id : new_ids)
    {
        if (cached.find(emoji_id) == cached.end())
        {
            bot.guild_emoji_get(guild_id, emoji_id, [this, guild_id, emoji_id](const dpp::confirmation_callback_t& callback)
            {
                if (callback.is_error()) return;

                dpp::emoji new_emoji = std::get<dpp::emoji>(callback.value);
                std::string mention = new_emoji.get_mention();

                sendLogWithAudit(guild_id, AuditLogEvent::EmojiCreate, emoji_id, "Emoji Created: ",
                                  ObjectType::Emoji, LogType::Success, mention);

                emoji_cache[guild_id][emoji_id] = new_emoji;
            });
        }
    }


    for (auto cached_it = cached.begin(); cached_it != cached.end(); )
    {
        if (new_ids.find(cached_it->first) == new_ids.end())
        {
            const dpp::emoji& removed_emoji = cached_it->second;
            std::string display_name = (removed_emoji.is_animated() ? "a:" : "") + removed_emoji.name;

            sendLogWithAudit(guild_id, AuditLogEvent::EmojiDelete, cached_it->first, "Emoji Deleted: ",
                              ObjectType::Emoji, LogType::Warning, display_name);

            cached_it = cached.erase(cached_it);
        }
        else
        {
            ++cached_it;
        }
    }
}



void ModLogModule::onMessageDelete(const dpp::message_delete_t& event)
{
    dpp::snowflake guild_id = event.guild_id;
    std::string details = "Channel: <#" + std::to_string(event.channel_id) + ">";

    //TODO
}









