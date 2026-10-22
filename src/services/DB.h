#include "dpp/dpp.h"
#include "sqlite3.h"

class DB
{
    private:

    sqlite3* db;

    public:

    explicit DB(sqlite3* handle) : db(handle) {}

    dpp::snowflake getColumnFromServerConfig(dpp::snowflake guild_id, std::string column) const;
    std::string getStringColumnFromServerConfig(dpp::snowflake guild_id, std::string column) const;

};
