#include "BotClient.h"
#include <iostream>
#include <cstdlib>

BotClient::BotClient() : dpp::cluster(
    std::getenv("VISORBOT_TOKEN") ? std::getenv("VISORBOT_TOKEN") : "",
    dpp::i_default_intents | dpp::i_message_content | dpp::i_guild_members
)
{

    if (sqlite3_open("VisorBot.db", &db) != SQLITE_OK) //TODO
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    }
    else
    {
        std::cout << "Successfully opened database" << std::endl;
        InitDatabase();
    }
}

BotClient::~BotClient()
{
    if (db)
    {
        sqlite3_close(db);
        std::cout << "Successfully closed database" << std::endl;
    }
}


void BotClient::InitDatabase()
{
    std::string sql = "CREATE TABLE IF NOT EXISTS ServerConfig ("
                      "GuildID TEXT PRIMARY KEY, "
                      "ModChannelID TEXT);"
                      "CREATE TABLE IF NOT EXISTS UserHasAdmin("
                      "UserID TEXT PRIMARY KEY,"
                      "HasPerms INTEGER);";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    addColumnIfNotExists("ServerConfig", "HelpChannelID", "TEXT");
    addColumnIfNotExists("ServerConfig", "IssueChannelID", "TEXT");
    addColumnIfNotExists("ServerConfig", "FaqChannelID", "TEXT");
    addColumnIfNotExists("ServerConfig", "IssueResponseChannelID", "TEXT");
}


void BotClient::addColumnIfNotExists(const std::string& table, const std::string& column, const std::string& type)
{
    std::string sql = "ALTER TABLE " + table + " ADD COLUMN " + column + " " + type + ";";
    char* errMsg = nullptr;

    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::string err_str = errMsg ? errMsg : "";
        if (err_str.find("duplicate column name") == std::string::npos)
        {
            std::cerr << "SQL error (ALTER TABLE " << table << "." << column << "): " << errMsg << std::endl;
        }
        sqlite3_free(errMsg);
    }
}

bool BotClient::userHasAdminPermission(dpp::snowflake user_id)
{
    std::string user_str = std::to_string(user_id);
    std::string query = "SELECT HasPerms FROM UserHasAdmin WHERE UserID = ?";
    sqlite3_stmt* stmt;
    bool has_perms = false;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, user_str.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            has_perms = sqlite3_column_int(stmt, 0) != 0;
        }

        sqlite3_finalize(stmt);
    }
    else
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db)<< std::endl;
    }
    return has_perms;
}
