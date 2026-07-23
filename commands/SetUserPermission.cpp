#include "SetUserPermission.h"
#include <sstream>
#include <iomanip>
#include "../BotClient.h"
#include <sqlite3.h>
#include <dpp/dpp.h>
#include <string>


std::string SetUserPermission::getName() const
{
    return "setuserpermission";
}
std::string SetUserPermission::getDescription() const
{
    return "Sets the users permission to use the Visor Bot";

}

void SetUserPermission::execute(const dpp::slashcommand_t& event)
{

    dpp::snowflake user_snowflake = (std::get<dpp::snowflake>(event.get_parameter("user")));
    bool has_perms = std::get<bool>(event.get_parameter("botpermissions"));
    std::string bool_str = has_perms ? "1" : "0";
    std::string user_str = std::to_string(user_snowflake);


    BotClient& client = static_cast<BotClient&>(bot);
    sqlite3* db = client.getDB();

    std::string query = "INSERT OR REPLACE INTO UserHasAdmin (UserID, HasPerms) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user_str.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, has_perms ? 1 : 0);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            event.reply("Permissions for <@" + user_str + "> have been set to: " + (has_perms ? "granted" : "revoked"));
        } else {
            event.reply("Error while saving in Database");
        }
        sqlite3_finalize(stmt);
    } else {
        event.reply("Internal DB error");
    }
}