#include "SetChannel.h"
#include "../BotClient.h"
#include <sqlite3.h>
#include <string>
#include <dpp/dpp.h>

std::string SetChannel::getName() const
{
    return "setchannel";
}

std::string SetChannel::getDescription() const
{
    return "Sets a channel type to a channel";
}

void SetChannel::execute(const dpp::slashcommand_t& event)
{
    std::string GuildID = std::to_string(event.command.guild_id);
    dpp::snowflake channel_snowflake = std::get<dpp::snowflake>(event.get_parameter("channel"));
    std::string channelID = std::to_string(channel_snowflake);


    std::string channelType = std::get<std::string>(event.get_parameter("channeltype"));


    std::string targetColumn = "";
    if (channelType == "Mod_Channel") targetColumn = "ModChannelID";
    else if (channelType == "Help_Channel") targetColumn = "HelpChannelID";
    else if (channelType == "Issue_Channel") targetColumn = "IssueChannelID";
    else if (channelType == "Faq_Channel") targetColumn = "FaqChannelID";
    else if (channelType == "IssueRspns_Channel") targetColumn = "IssueResponseChannelID";


    if (targetColumn.empty()) {
        event.reply(dpp::message("Incorrect ChannelType").set_flags(dpp::m_ephemeral));
        return;
    }

    BotClient& client = static_cast<BotClient&>(bot);
    sqlite3* db = client.getDB();


    std::string query = "INSERT INTO ServerConfig (GuildID, " + targetColumn + ") VALUES (?, ?) "
                        "ON CONFLICT(GuildID) DO UPDATE SET " + targetColumn + " = excluded." + targetColumn + ";";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, GuildID.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, channelID.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            std::string replyMsg =   channelType + " was successfully set to <#" + channelID + ">";
            event.reply(dpp::message(replyMsg).set_flags(dpp::m_ephemeral));
        } else {
            event.reply(dpp::message("Error while saving in DB").set_flags(dpp::m_ephemeral));
        }
    } else {
        event.reply(dpp::message("Internal DB error").set_flags(dpp::m_ephemeral));
    }

    sqlite3_finalize(stmt);
}