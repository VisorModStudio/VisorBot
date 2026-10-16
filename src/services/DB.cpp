#include "DB.h"

dpp::snowflake DB::getColumnFromServerConfig(dpp::snowflake guild_id, std::string column) const
{
    dpp::snowflake channel_id = 0;


    std::string sql = "SELECT " + column + " FROM ServerConfig WHERE GuildID = ?;";
    sqlite3_stmt* stmt = nullptr;


    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int64(stmt, 1, static_cast<int64_t>(guild_id));

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            channel_id = static_cast<dpp::snowflake>(sqlite3_column_int64(stmt, 0));
        }
    }


    sqlite3_finalize(stmt);

    return channel_id;
}

std::string DB::getStringColumnFromServerConfig(dpp::snowflake guild_id, std::string column) const
{
    std::string result = "";

    std::string sql = "SELECT " + column + " FROM ServerConfig WHERE GuildID = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int64(stmt, 1, static_cast<int64_t>(guild_id));

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const unsigned char* text = sqlite3_column_text(stmt, 0);

            if (text != nullptr) {
                result = reinterpret_cast<const char*>(text);
            }
        }
    }

    sqlite3_finalize(stmt);

    return result;
}

