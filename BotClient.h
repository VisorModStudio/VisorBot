#pragma once
#include <dpp/dpp.h>
#include <sqlite3.h>
#include <map>
#include <memory>
#include "Command.h"

class BotClient : public dpp::cluster
{
public:
    BotClient();
    ~BotClient();

    sqlite3* getDB() const { return db; }

private:
    sqlite3* db = nullptr;
    void InitDatabase();
};