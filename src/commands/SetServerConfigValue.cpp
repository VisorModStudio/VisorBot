#include "SetServerConfigValue.h"
#include "../src/BotClient.h"
#include <sqlite3.h>
#include <string>
#include <dpp/dpp.h>

std::string SetServerConfigValue::getName() const
{
    return "setdbvalue";
}

std::string SetServerConfigValue::getDescription() const
{
    return "Sets values for specific db entries";
}

void SetServerConfigValue::execute(const dpp::slashcommand_t& event)
{
    std::string GuildID = std::to_string(event.command.guild_id);


    dpp::snowflake channel_snowflake = 0;
    dpp::snowflake role_snowflake = 0;
    std::string text_value = "";


    auto channel_param = event.get_parameter("channel_value");
    if (auto* id = std::get_if<dpp::snowflake>(&channel_param)) {
        channel_snowflake = *id;
    }

    auto role_param = event.get_parameter("role_value");
    if (auto* id = std::get_if<dpp::snowflake>(&role_param)) {
        role_snowflake = *id;
    }

    auto text_param = event.get_parameter("text_value");
    if (auto* str = std::get_if<std::string>(&text_param)) {
        text_value = *str;
    }

    std::string entry_type = std::get<std::string>(event.get_parameter("entrytype"));


    std::string targetColumn = "";
    if (entry_type == "Mod_Channel") targetColumn = "ModChannelID";
    else if (entry_type == "Help_Channel") targetColumn = "HelpChannelID";
    else if (entry_type == "Issue_Channel") targetColumn = "IssueChannelID";
    else if (entry_type == "Faq_Channel") targetColumn = "FaqChannelID";
    else if (entry_type == "IssueRspns_Channel") targetColumn = "IssueResponseChannelID";
    else if (entry_type == "BotAccess_Role") targetColumn = "BotAccessRole";


    if (targetColumn.empty()) {
        event.reply(dpp::message("Incorrect ChannelType").set_flags(dpp::m_ephemeral));
        return;
    }


    int values_set = 0;
    if (channel_snowflake != 0) values_set++;
    if (role_snowflake != 0) values_set++;
    if (!text_value.empty()) values_set++;

    if (values_set > 1) {
        event.reply(dpp::message("Please provide EXACTLY ONE value (channel, role, OR text/URL)!").set_flags(dpp::m_ephemeral));
        return;
    }
    if (values_set == 0) {
        event.reply(dpp::message("At least one value (channel, role, or text) must be set!").set_flags(dpp::m_ephemeral));
        return;
    }

    std::string valueToStore = "";
    std::string replyMsg = "";

    if (channel_snowflake != 0) {
        valueToStore = std::to_string(channel_snowflake);
        replyMsg = entry_type + " was successfully set to <#" + valueToStore + ">";
    } else if (role_snowflake != 0) {
        valueToStore = std::to_string(role_snowflake);
        replyMsg = entry_type + " was successfully set to <@&" + valueToStore + ">";
    } else {
        valueToStore = text_value;
        replyMsg = entry_type + " was successfully set to `" + text_value + "`";
    }


    BotClient& client = static_cast<BotClient&>(bot);
    sqlite3* db = client.getDB();

    std::string query = "INSERT INTO ServerConfig (GuildID, " + targetColumn + ") VALUES (?, ?) "
                        "ON CONFLICT(GuildID) DO UPDATE SET " + targetColumn + " = excluded." + targetColumn + ";";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, GuildID.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, valueToStore.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            event.reply(dpp::message(replyMsg).set_flags(dpp::m_ephemeral));
        } else {
            event.reply(dpp::message("Error while saving in DB").set_flags(dpp::m_ephemeral));
        }
    }
    else
    {
        event.reply(dpp::message("Internal DB error").set_flags(dpp::m_ephemeral));
    }

    sqlite3_finalize(stmt);
}



