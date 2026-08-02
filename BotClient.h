#pragma once
#include <dpp/dpp.h>
#include <sqlite3.h>


class BotClient : public dpp::cluster
{
public:
    BotClient();
    ~BotClient();

    sqlite3* getDB() const { return db; }

    bool userHasAdminPermission(dpp::snowflake user_id);
    void addColumnIfNotExists(const std::string& table, const std::string& column, const std::string& type);


private:
    sqlite3* db = nullptr;
    void InitDatabase();
};