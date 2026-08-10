#include "SetDbValue.h"
#include "../BotClient.h"
#include <sqlite3.h>
#include <string>
#include <dpp/dpp.h>

std::string SetDbValue::getName() const
{
    return "setdbvalue";
}

std::string SetDbValue::getDescription() const
{
    return "Sets values for specific db entries";
}

void SetDbValue::execute(const dpp::slashcommand_t& event)
{
    std::string GuildID = std::to_string(event.command.guild_id);


    dpp::snowflake channel_snowflake = 0;
    std::string text_value = "";


    auto channel_param = event.get_parameter("channel_value");
    if (auto* id = std::get_if<dpp::snowflake>(&channel_param)) {
        channel_snowflake = *id;
    }


    auto text_param = event.get_parameter("text_value");
    if (auto* str = std::get_if<std::string>(&text_param)) {
        text_value = *str;
    }

    std::string channelType = std::get<std::string>(event.get_parameter("entrytype"));


    std::string targetColumn = "";
    if (channelType == "Mod_Channel") targetColumn = "ModChannelID";
    else if (channelType == "Help_Channel") targetColumn = "HelpChannelID";
    else if (channelType == "Issue_Channel") targetColumn = "IssueChannelID";
    else if (channelType == "Faq_Channel") targetColumn = "FaqChannelID";
    else if (channelType == "IssueRspns_Channel") targetColumn = "IssueResponseChannelID";
    else if (channelType == "Faq_Url") targetColumn = "FaqUrl";

    if (targetColumn.empty()) {
        event.reply(dpp::message("Incorrect ChannelType").set_flags(dpp::m_ephemeral));
        return;
    }


    std::string valueToStore = "";
    std::string replyMsg = "";

    if (channel_snowflake != 0 && text_value.empty())
    {

        valueToStore = std::to_string(channel_snowflake);
        replyMsg = channelType + " was successfully set to <#" + valueToStore + ">";
    }
    else if (channel_snowflake == 0 && !text_value.empty())
    {

        valueToStore = text_value;
        replyMsg = channelType + " was successfully set to `" + text_value + "`";
    }
    else if (channel_snowflake != 0 && !text_value.empty())
    {

        event.reply(dpp::message("Please provide EITHER a channel OR text/URL, not both!").set_flags(dpp::m_ephemeral));
        return;
    }
    else
    {

        event.reply(dpp::message("At least one value (channel or text) must be set!").set_flags(dpp::m_ephemeral));
        return;
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