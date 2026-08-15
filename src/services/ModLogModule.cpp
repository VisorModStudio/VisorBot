#include "ModLogModule.h"
#include <string>
#include <thread>
#include "GeminiClient.h"
#include "Logfetcher.h"




dpp::snowflake ModLogModule::getColumnFromServerConfig(dpp::snowflake guild_id, std::string column) const
{
    dpp::snowflake channel_id = 0;


    std::string sql = "SELECT " + column + " FROM ServerConfig WHERE GuildID = ?;";
    sqlite3_stmt* stmt = nullptr;


    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
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

std::string ModLogModule::getStringColumnFromServerConfig(dpp::snowflake guild_id, std::string column) const
{
    std::string result = "";

    std::string sql = "SELECT " + column + " FROM ServerConfig WHERE GuildID = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int64(stmt, 1, static_cast<int64_t>(guild_id));

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const unsigned char* text = sqlite3_column_text(stmt, 0);

            if (text != nullptr) {
                result = reinterpret_cast<const char*>(text);
            }
        }
    }

    sqlite3_finalize(stmt);

    return result;
}


void ModLogModule::registerHandlers() {

    bot.on_button_click([this](const dpp::button_click_t& event)
    {
        onButtonClick(event);
    });

    bot.on_select_click([this](const dpp::select_click_t& event)
    {
        onSelectClick(event);
    });

    bot.on_guild_ban_add([this](const dpp::guild_ban_add_t& event)
    {
        onMemberBan(event);
    });

    bot.on_guild_role_create([this](const dpp::guild_role_create_t& event) {
        onRoleCreate(event);
    });

    bot.on_guild_role_delete([this](const dpp::guild_role_delete_t& event) {
        onRoleDelete(event);
    });

    bot.on_channel_create([this](const dpp::channel_create_t& event)
    {
        onChannelCreate(event);
    });

    bot.on_channel_delete([this](const dpp::channel_delete_t& event)
    {
        onChannelDelete(event);
    });

    bot.on_channel_update([this](const dpp::channel_update_t& event)
    {
        onChannelUpdate(event);
    });

    bot.on_invite_create([this](const dpp::invite_create_t& event)
    {
        onInviteCreate(event);
    });

    bot.on_invite_delete([this](const dpp::invite_delete_t& event)
    {
        onInviteDelete(event);
    });

    //TODO Webhook

    bot.on_guild_emojis_update([this](const dpp::guild_emojis_update_t& event)
    {
        onGuildEmojisUpdate(event);
    });

    bot.on_message_delete([this](const dpp::message_delete_t& event) {
        onMessageDelete(event);
    });

    bot.on_message_create([this](const dpp::message_create_t& event)
    {
        onMessageCreate(event);
    });

    bot.on_message_delete_bulk([this](const dpp::message_delete_bulk_t& event)
    {
        onMessageBulkDelete(event);
    });

    bot.on_thread_create([this](const dpp::thread_create_t& event)
    {
        onThreadCreate(event);
    });





    /*
    bot.on_guild_member_add([this](const dpp::guild_member_add_t& event) {
        onMemberJoin(event);

        //TODO
    });
    */
}

void ModLogModule::onButtonClick(const dpp::button_click_t& event)
{
    std::string custom_id = event.custom_id;
    //TODO maybe add AI + message context


    size_t sep = custom_id.find(':');
    if (sep == std::string::npos) return;

    std::string action = custom_id.substr(0, sep);
    dpp::snowflake thread_id = std::stoull(custom_id.substr(sep + 1));

    dpp::embed embed = dpp::embed();
    dpp::message msg;
    msg.set_channel_id(thread_id);
    dpp::component row;


    if (action == "info_ask_button")
    {
        embed.set_color(dpp::colors::blurple)
        .set_title("Information Request")
        .set_description("Please send more information about the problem like mods list and other info that might help to find the cause");

    }
    else if (action == "log_ask_button")
    {
        embed.set_color(dpp::colors::blurple)
        .set_title("Game Log Request")
        .set_description("Please Post your Game Log! You can find it here: ``` YourMinecraftDirectory/logs/latest.log``` Or you can select your launcher here to get a Detailed description:");

        dpp::component launcher_select_menu;

        launcher_select_menu.set_type(dpp::cot_selectmenu)
                            .set_id("launcher_select_menu")
                            .set_placeholder("please select your launcher");

        launcher_select_menu.add_select_option(dpp::select_option("Prism Launcher", "prism_launcher", ""))
                            .add_select_option(dpp::select_option("Curse Forge", "curse_forge", ""))
                            .add_select_option(dpp::select_option("Modrinth App", "modrinth", ""))
                            .add_select_option(dpp::select_option("ATlauncher", "atlauncher", ""))
                            .add_select_option(dpp::select_option("Minecraft Launcher", "minecraft_launcher", ""));

        row.add_component(launcher_select_menu);
        msg.add_component(row);
    }

    msg.add_embed(embed);
    bot.message_create(msg);

    event.reply(dpp::message("Request sent to the thread!").set_flags(dpp::m_ephemeral));
}

void ModLogModule::onSelectClick(const dpp::select_click_t& event)
{
    std::string select_id = event.custom_id;
    std::string selected_option_id = event.values[0];
    dpp::embed embed;
    dpp::message msg;

    if (select_id.empty()) return;

    else if (select_id == "launcher_select_menu")
    {
        if (selected_option_id == "prism_launcher")
        {
            embed.set_title("Prism Launcher")
                .set_description(R"(**1.** Open the Prism Launcher
                                    **2.** Right-click on your modpack and click **Edit**.
                                    **3.** Press **Copy**.)");
        }
        else if (selected_option_id == "curse_forge")
        {

            embed.set_title("Curse Forge")
                .set_description(R"(**1.** Open the Curse Forge App
                                    **2.** Open on your modpack and click the 3 dots next to the **Play** button
                                    **3.** Click **Open Folder** and go into the logs folder
                                    **4.** Copy the **latest.log** file)");

        }
        else if (selected_option_id == "modrinth")
        {
            embed.set_title("Modrinth App")
                .set_description(R"(**1.** Open the Modrinth App
                                    **2.** In the instances list, click your instance.
                                    **3.** Navigate to the **Logs** tab.
                                    **4.** Select the **latest.log** file the dropdown list.
                                    **5.** Click **Copy** )");
        }
        else if (selected_option_id == "atlauncher")
        {
            embed.set_title("Atlauncher")
                .set_description(R"(**1.** Open the AtLauncher
                                    **2.** Click on the **Instances** Tab
                                    **3.** Locate your Instance and click **Open Folder**.
                                    **4.** Open the **logs** folder and right-click the **latest.log** file.
                                    **5.** Press **Copy** )");
        }
        else if (selected_option_id == "minecraft_launcher")
        {
            embed.set_title("Minecraft Launcher")
                .set_description(R"(**1.** Press Windows + R
                                    **2.** Type %appdata%
                                    **3.** Open .minecraft
                                    **4.** Open the **logs** folder and locate latest.log
                                    **5.** Right-Click the file and press **Copy**)");
        }

        msg.add_embed(embed);
        msg.set_flags(dpp::m_ephemeral);
        event.reply(dpp::message(msg));

    }

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
    dpp::snowflake channel_id = getColumnFromServerConfig(guild_id, "ModChannelID");

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

void ModLogModule::onChannelCreate(const dpp::channel_create_t& event)
{
    dpp::snowflake guild_id = event.creating_guild.id;
    std::string channel_name = event.created.name;
    dpp::snowflake channel_id = event.created.id;

    sendLogWithAudit(guild_id, AuditLogEvent::ChannelCreate, channel_id, "Channel Created: ", ObjectType::Channel, LogType::Success);
}

void ModLogModule::onChannelUpdate(const dpp::channel_update_t& event)
{
    dpp::snowflake guild_id = event.updating_guild.id;
    std::string channel_name = event.updated.name;
    dpp::snowflake channel_id = event.updated.id;

    sendLogWithAudit(guild_id, AuditLogEvent::ChannelUpdate, channel_id, "Channel Updated: ", ObjectType::Channel, LogType::Success);
}

void ModLogModule::onChannelDelete(const dpp::channel_delete_t& event)
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

void ModLogModule::onRoleCreate(const dpp::guild_role_create_t& event)
{
    dpp::snowflake guild_id = event.creating_guild.id;
    dpp::snowflake role_id = event.created.id;
    std::string role_name = event.created.name;

    sendLogWithAudit(guild_id, AuditLogEvent::RoleCreate, role_id, "Role Created:", ObjectType::Role, LogType::Success);
}

void ModLogModule::onRoleDelete(const dpp::guild_role_delete_t& event)
{
    dpp::snowflake guild_id = event.deleting_guild.id;
    std::string role_name = event.deleted.name;
    dpp::snowflake role_id = event.deleted.id;

    sendLogWithAudit(guild_id ,AuditLogEvent::RoleDelete, role_id, "Role Deleted: ", ObjectType::Role, LogType::Warning);
}

void ModLogModule::onInviteCreate(const dpp::invite_create_t& event)
{
    dpp::snowflake guild_id = event.created_invite.guild_id;
    std::string invite_code = event.created_invite.code;
    dpp::snowflake inviter_id = event.created_invite.inviter_id;

    sendLog(guild_id, LogType::Success, "Invite Created:", "By: <@" + inviter_id.str() + ">\n" + "Invite Code: " + invite_code);
}

void ModLogModule::onInviteDelete(const dpp::invite_delete_t& event)
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


void ModLogModule::onMessageCreate(const dpp::message_create_t& event)
{
    dpp::snowflake msg_id = event.msg.id;
    std::string msg_content = event.msg.content;
    dpp::snowflake author_id = event.msg.author.id;

    message_cache.insert({msg_id, {author_id, msg_content}});
}

void ModLogModule::onMessageDelete(const dpp::message_delete_t& event)
{
    dpp::snowflake guild_id = event.guild_id;
    dpp::snowflake msg_id = event.id;
    dpp::snowflake channel_id = event.channel_id;



    auto it = message_cache.find(msg_id);
    std::string msg_content;
    dpp::snowflake author_id;

    if (it != message_cache.end())
    {
        author_id = it->second.first;
        msg_content = it->second.second;

        sendLog(guild_id, LogType::Warning, "Message Deleted: ",
                "Content: " + msg_content + "\nSent By: <@" + author_id.str() + ">\nChannel: <#" + channel_id.str() + ">" );

    }
    else
    {
        std::cerr << "Msg not cached";
    }

}

void ModLogModule::onMessageBulkDelete(const dpp::message_delete_bulk_t& event)
{
    dpp::snowflake guild_id = event.deleting_guild.id;
    dpp::snowflake channel_id = event.deleting_channel.id;
    std::vector<dpp::snowflake> msg_list = event.deleted;

    dpp::snowflake log_channel_id = getColumnFromServerConfig(guild_id, "ModChannelID");
    if (log_channel_id == 0) return;

    dpp::embed current_embed = dpp::embed()
        .set_title("Bulk Delete: " + std::to_string(msg_list.size()) + " Messages in <#" + channel_id.str() + ">")
        .set_color(dpp::colors::orange)
        .set_timestamp(time(nullptr));

    int field_count = 0;
    size_t char_count = 0;
    int not_cached = 0;

    auto flush_embed = [&]()
    {
        if (field_count > 0)
        {
            bot.message_create(dpp::message(log_channel_id, current_embed));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        current_embed = dpp::embed()
            .set_title("Bulk Delete (Continue)")
            .set_color(dpp::colors::orange)
            .set_timestamp(time(nullptr));
        field_count = 0;
        char_count = 0;
    };

    for (const dpp::snowflake& msg_id : msg_list)
    {
        auto it = message_cache.find(msg_id);
        if (it == message_cache.end())
        {
            not_cached++;
            continue;
        }

        dpp::snowflake author_id = it->second.first;
        std::string content = it->second.second.empty() ? "*No Text*" : it->second.second;

        size_t offset = 0;
        do
        {
            std::string chunk_text = content.substr(offset, 900);
            offset += 900;

            std::string field_name = "Message";
            std::string field_value = "Sent By: <@" + author_id.str() + ">\nContent: " + chunk_text;

            if (field_count >= 25 || char_count + field_value.size() + field_name.size() > 5800)
            {
                flush_embed();
            }

            current_embed.add_field(field_name, field_value);
            field_count++;
            char_count += field_value.size() + field_name.size();

        } while (offset < content.size());
    }

    if (not_cached > 0)
    {
        current_embed.add_field("Not Cached", std::to_string(not_cached) + "Message(s) could not be reconstructed");
        field_count++;
    }

    flush_embed();
}


void ModLogModule::onThreadCreate(const dpp::thread_create_t& event)
{
    dpp::snowflake thread_id = event.created.id;
    dpp::snowflake thread_creator = event.created.owner_id;
    dpp::snowflake parent_id = event.created.parent_id;
    dpp::snowflake guild_id = event.created.guild_id;

    dpp::snowflake help_id = getColumnFromServerConfig(guild_id, "HelpChannelID");
    dpp::snowflake issue_id = getColumnFromServerConfig(guild_id, "IssueChannelID");
    dpp::snowflake response_channel_id = getColumnFromServerConfig(guild_id, "IssueResponseChannelID");


    if (parent_id != issue_id && parent_id != help_id) {
        return;
    }


    if (processed_threads.contains(thread_id)) {
        return;
    }
    processed_threads.insert(thread_id);

    std::string thread_title = event.created.name;


    if (parent_id == issue_id)
    {
        bot.message_get(thread_id, thread_id, [this, thread_id, thread_title, thread_creator, response_channel_id](const dpp::confirmation_callback_t& cb) {
            if (cb.is_error()) {
                processed_threads.erase(thread_id);
                return;
            }

            auto msg = std::get<dpp::message>(cb.value);

            gemini.summarize_post(thread_title, msg.content, [this, thread_id, thread_creator, response_channel_id](std::string summary) {
                dpp::embed embed = dpp::embed()
                    .set_color(dpp::colors::blurple)
                    .set_title("New Issue")
                    .set_description(summary + "\n Created By: <@" + thread_creator.str() + ">\n Post: <#" + thread_id.str() + ">");

                dpp::message msg(response_channel_id, embed);

                msg.add_component(
                    dpp::component().add_component(
                        dpp::component()
                            .set_label("Ask for Information")
                            .set_type(dpp::cot_button)
                            .set_emoji("❔")
                            .set_style(dpp::cos_primary)
                            .set_id("info_ask_button:" + thread_id.str())
                    ).add_component(
                        dpp::component()
                            .set_label("Ask for game log")
                            .set_type(dpp::cot_button)
                            .set_emoji("❓")
                            .set_style(dpp::cos_primary)
                            .set_id("log_ask_button:" + thread_id.str())
                    )
                );

                bot.message_create(msg);
            });
        });
    }

    else if (parent_id == help_id)
    {

        std::string faq_content = knowledgeCache.getCombinedContext(guild_id);

        if (faq_content.empty()) {
            processed_threads.erase(thread_id);
            return;
        }


        bot.message_get(thread_id, thread_id, [this, thread_id, thread_title, faq_content](const dpp::confirmation_callback_t& cb) {
            if (cb.is_error()) {
                processed_threads.erase(thread_id);
                return;
            }

            dpp::message msg = std::get<dpp::message>(cb.value);
            std::vector<dpp::attachment> attachments = msg.attachments;


            if (attachments.empty()) {
                gemini.answer_faq(
                    "Question title: " + thread_title + "\nQuestion content: " + msg.content,
                    faq_content,
                    [this, thread_id](std::string answer) {
                        dpp::embed embed = dpp::embed()
                            .set_color(dpp::colors::blurple)
                            .set_title("Visor-Wiki")
                            .set_description(answer);
                        bot.message_create(dpp::message(thread_id, embed));
                    }
                );
                return;
            }


            auto logContents = std::make_shared<std::vector<std::string>>(attachments.size());
            auto remaining = std::make_shared<std::atomic<int>>(static_cast<int>(attachments.size()));

            for (size_t i = 0; i < attachments.size(); i++) {
                const std::string url = attachments[i].url;

                fetchAndCleanLog(bot, url, [this, thread_id, thread_title, msg, faq_content, logContents, remaining, i](LogFetchResult result) {
                    if (result.success && !result.content.empty()) {
                        (*logContents)[i] = result.content;
                    }


                    if (--(*remaining) == 0) {
                        std::string combinedLogs;
                        for (const auto& content : *logContents) {
                            if (!content.empty()) {
                                combinedLogs += content + "\n";
                            }
                        }

                        std::string question = "Question title: " + thread_title +
                                                "\nQuestion content: " + msg.content;

                        if (!combinedLogs.empty()) {
                            question += "\nAttached log content:\n" + combinedLogs;
                        }

                        gemini.answer_faq(
                            question,
                            faq_content,
                            [this, thread_id](std::string answer) {
                                dpp::embed embed = dpp::embed()
                                    .set_color(dpp::colors::blurple)
                                    .set_title("Visor-Wiki")
                                    .set_description(answer);
                                bot.message_create(dpp::message(thread_id, embed));
                            }
                        );
                    }
                });
            }
        });
    }
}









