#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <sqlite3.h>
#include <dpp/dpp.h>

struct KnowledgeSource {
    int id;
    std::string name;
    std::string url;
};

class KnowledgeCache {
private:
    std::unordered_map<dpp::snowflake, std::string> cache;
    mutable std::mutex cache_mutex;

public:
    KnowledgeCache() = default;
    ~KnowledgeCache() = default;


    void reloadGuild(dpp::snowflake guild_id, sqlite3* db);
    void reloadAll(sqlite3* db);
    std::string getCombinedContext(dpp::snowflake guild_id) const;


    static bool addSource(sqlite3* db, dpp::snowflake guild_id, const std::string& name, const std::string& url);
    static bool removeSource(sqlite3* db, int source_id);
    static std::vector<KnowledgeSource> getSources(sqlite3* db, dpp::snowflake guild_id);

    static bool findSourceByUrl(sqlite3* db, dpp::snowflake guild_id, const std::string& url, std::string& out_name);
    static bool sourceNameExists(sqlite3* db, dpp::snowflake guild_id, const std::string& name);
    static bool updateSourceUrl(sqlite3* db, dpp::snowflake guild_id, const std::string& name, const std::string& url);
};