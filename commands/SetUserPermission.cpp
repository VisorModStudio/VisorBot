#include "SetUserPermission.h"
#include <sstream>
#include <iomanip>
#include "../BotClient.h"
#include <sqlite3.h>
#include <dpp/dpp.h>
#include <string>


std::string SetUserPermission::getName() const
{
    return "SetUserPermission";
}
std::string SetUserPermission::getDescription() const
{
    return "Sets the users permission to use the Visor Bot";

}

void SetUserPermission::execute(const dpp::slashcommand_t& event)
{

    dpp::snowflake user_snowflake = (std::get<dpp::snowflake>(event.get_parameter("User")));
    dpp::snowflake bool_snowflake = (std::get<dpp::snowflake>(event.get_parameter("BotPermissions")));
    std::string user_str = std::to_string(user_snowflake);
    std::string bool_str = std::to_string(bool_snowflake);

    BotClient& client = static_cast<BotClient&>(bot);
    sqlite3* db = client.getDB();

    std::string query = "INSERT OR REPLACE INTO UserHasAdmin (UserID, HasPerms) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user_str.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, bool_str.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            event.reply("ModChannel got registered successfully.");
        } else {
            event.reply("Error while saving in Databaste");
        }
    } else {
        event.reply("Internal DB error");
    }

    sqlite3_finalize(stmt);
    event.reply("Use Permissions have been set for: @"+ bool_str);
}

