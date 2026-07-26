#include "SetModChannel.h"
#include "../BotClient.h"
#include <sqlite3.h>
#include <string>
#include <dpp/dpp.h>

std::string SetModChannel::getName() const
{
    return "setmodchannel";
}

std::string SetModChannel::getDescription() const
{
    return "Sets the channel where the bot posts Admin relevant stuff";
}

void SetModChannel::execute(const dpp::slashcommand_t& event)
{
    std::string GuildID = std::to_string(event.command.guild_id);
    dpp::snowflake channel_snowflake = std::get<dpp::snowflake>(event.get_parameter("channel"));
    std::string channelID = std::to_string(channel_snowflake);

    BotClient& client = static_cast<BotClient&>(bot);
    sqlite3* db = client.getDB();

    std::string query = "INSERT OR REPLACE INTO ServerConfig (GuildID, ModChannelID) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, GuildID.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, channelID.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            event.reply(dpp::message("ModChannel got registered successfully.").set_flags(dpp::m_ephemeral));
        } else {
            event.reply(dpp::message("Error while saving in Databaste").set_flags(dpp::m_ephemeral));
        }
    } else {
        event.reply(dpp::message("Internal DB error").set_flags(dpp::m_ephemeral));
    }

    sqlite3_finalize(stmt);
}