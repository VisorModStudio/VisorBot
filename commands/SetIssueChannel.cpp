#include "SetIssueChannel.h"
#include "../BotClient.h"
#include <sqlite3.h>
#include <string>
#include <dpp/dpp.h>

std::string SetIssueChannel::getName() const
{
    return "setissuechannel";
}

std::string SetIssueChannel::getDescription() const
{
    return "Sets the channel where the bot posts issue-relevant stuff";
}

void SetIssueChannel::execute(const dpp::slashcommand_t& event)
{
    std::string GuildID = std::to_string(event.command.guild_id);
    dpp::snowflake channel_snowflake = std::get<dpp::snowflake>(event.get_parameter("channel"));
    std::string channelID = std::to_string(channel_snowflake);

    BotClient& client = static_cast<BotClient&>(bot);
    sqlite3* db = client.getDB();

    std::string query = "INSERT INTO ServerConfig (GuildID, IssueChannelID) VALUES (?, ?) "
                     "ON CONFLICT(GuildID) DO UPDATE SET IssueChannelID = excluded.IssueChannelID;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, GuildID.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, channelID.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            event.reply(dpp::message("IssueChannel got registered successfully.").set_flags(dpp::m_ephemeral));
        } else {
            event.reply(dpp::message("Error while saving in Database").set_flags(dpp::m_ephemeral));
        }
    } else {
        event.reply(dpp::message("Internal DB error").set_flags(dpp::m_ephemeral));
    }

    sqlite3_finalize(stmt);
}