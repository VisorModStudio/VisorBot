#include "BotClient.h"
#include <iostream>
#include <cstdlib>

BotClient::BotClient() : dpp::cluster(std::getenv("VISORBOT_TOKEN") ? std::getenv("VISORBOT_TOKEN") : "")
{
    if (sqlite3_open("ChannelDB.db", &db) != SQLITE_OK)
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
    // Hinweis: Deine CLion UI hat "ServerConfig" mit den Spalten "GuildID" und "ModChannelID" erstellt.
    // Wir passen das SQL-Statement hier an deine tatsächliche DB-Struktur aus CLion an!
    std::string sql = "CREATE TABLE IF NOT EXISTS ServerConfig ("
                      "GuildID TEXT PRIMARY KEY, "
                      "ModChannelID TEXT);";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}