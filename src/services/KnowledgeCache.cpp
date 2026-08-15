#define NOMINMAX

#include "KnowledgeCache.h"
#include <cpr/cpr.h>
#include <iostream>

void KnowledgeCache::reloadGuild(dpp::snowflake guild_id, sqlite3* db) {
    std::vector<KnowledgeSource> sources = getSources(db, guild_id);

    std::string combined_text = "";
    for (const auto& source : sources) {
        std::cout << "  Fetching knowledge source '" << source.name << "' (" << source.url << ")..." << std::endl;

        cpr::Response r = cpr::Get(
            cpr::Url{source.url},
            cpr::Header{{"User-Agent", "VisorBot/1.0"}},
            cpr::ConnectTimeout{5000},
            cpr::Timeout{15000}
        );

        if (r.error.code != cpr::ErrorCode::OK) {
            std::cerr << "  Failed to fetch '" << source.name << "': " << r.error.message << std::endl;
            continue;
        }

        if (r.status_code == 200) {
            combined_text += "\n=== SOURCE: " + source.name + " ===\n";
            combined_text += r.text + "\n";
        } else {
            std::cerr << "  Source '" << source.name << "' returned HTTP " << r.status_code << std::endl;
        }
    }

    std::lock_guard<std::mutex> lock(cache_mutex);
    cache[guild_id] = combined_text;
}

void KnowledgeCache::reloadAll(sqlite3* db) {

    std::string sql = "SELECT DISTINCT GuildID FROM KnowledgeSources;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* guild_text = sqlite3_column_text(stmt, 0);
            if (guild_text) {
                try {
                    dpp::snowflake guild_id = std::stoull(reinterpret_cast<const char*>(guild_text));
                    std::cout << "Reloading knowledge sources for guild " << guild_id << "..." << std::endl;
                    reloadGuild(guild_id, db);
                } catch (...) {}
            }
        }
    }
    sqlite3_finalize(stmt);
}

std::string KnowledgeCache::getCombinedContext(dpp::snowflake guild_id) const {
    std::lock_guard<std::mutex> lock(cache_mutex);
    auto it = cache.find(guild_id);
    if (it != cache.end()) {
        return it->second;
    }
    return "";
}

bool KnowledgeCache::addSource(sqlite3* db, dpp::snowflake guild_id, const std::string& name, const std::string& url) {
    std::string sql = "INSERT INTO KnowledgeSources (GuildID, SourceName, Url) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        std::string guild_str = std::to_string(guild_id);
        sqlite3_bind_text(stmt, 1, guild_str.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, url.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
    }
    sqlite3_finalize(stmt);
    return success;
}

bool KnowledgeCache::removeSource(sqlite3* db, int source_id) {
    std::string sql = "DELETE FROM KnowledgeSources WHERE SourceID = ?;";
    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, source_id);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
    }
    sqlite3_finalize(stmt);
    return success;
}

std::vector<KnowledgeSource> KnowledgeCache::getSources(sqlite3* db, dpp::snowflake guild_id) {
    std::vector<KnowledgeSource> sources;
    std::string sql = "SELECT SourceID, SourceName, Url FROM KnowledgeSources WHERE GuildID = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        std::string guild_str = std::to_string(guild_id);
        sqlite3_bind_text(stmt, 1, guild_str.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* name = sqlite3_column_text(stmt, 1);
            const unsigned char* url = sqlite3_column_text(stmt, 2);

            if (name && url) {
                sources.push_back({
                    id,
                    reinterpret_cast<const char*>(name),
                    reinterpret_cast<const char*>(url)
                });
            }
        }
    }
    sqlite3_finalize(stmt);
    return sources;
}

bool KnowledgeCache::findSourceByUrl(sqlite3* db, dpp::snowflake guild_id, const std::string& url, std::string& out_name) {
    std::string sql = "SELECT SourceName FROM KnowledgeSources WHERE GuildID = ? AND Url = ?;";
    sqlite3_stmt* stmt = nullptr;
    bool found = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        std::string guild_str = std::to_string(guild_id);
        sqlite3_bind_text(stmt, 1, guild_str.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, url.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* name = sqlite3_column_text(stmt, 0);
            if (name) {
                out_name = reinterpret_cast<const char*>(name);
                found = true;
            }
        }
    }
    sqlite3_finalize(stmt);
    return found;
}

bool KnowledgeCache::sourceNameExists(sqlite3* db, dpp::snowflake guild_id, const std::string& name) {
    std::string sql = "SELECT 1 FROM KnowledgeSources WHERE GuildID = ? AND SourceName = ?;";
    sqlite3_stmt* stmt = nullptr;
    bool found = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        std::string guild_str = std::to_string(guild_id);
        sqlite3_bind_text(stmt, 1, guild_str.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            found = true;
        }
    }
    sqlite3_finalize(stmt);
    return found;
}

bool KnowledgeCache::updateSourceUrl(sqlite3* db, dpp::snowflake guild_id, const std::string& name, const std::string& url) {
    std::string sql = "UPDATE KnowledgeSources SET Url = ? WHERE GuildID = ? AND SourceName = ?;";
    sqlite3_stmt* stmt = nullptr;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        std::string guild_str = std::to_string(guild_id);
        sqlite3_bind_text(stmt, 1, url.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, guild_str.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, name.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            success = true;
        }
    }
    sqlite3_finalize(stmt);
    return success;
}