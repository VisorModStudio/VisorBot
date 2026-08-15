#include "SetKnowledgeSource.h"
#include "../src/BotClient.h"
#include "../services/KnowledgeCache.h"
#include <sqlite3.h>
#include <string>
#include <dpp/dpp.h>
#include "../services/UrlSafety.h"

std::string SetKnowledgeSource::getName() const
{
    return "setknowledgesource";
}

std::string SetKnowledgeSource::getDescription() const
{
    return "set a new knowledge source URL for the AI.";
}

void SetKnowledgeSource::execute(const dpp::slashcommand_t& event)
{
    event.thinking(true);

    dpp::snowflake guild_id = event.command.guild_id;

    std::string source_name = "";
    std::string source_url = "";

    auto name_param = event.get_parameter("name");
    if (auto* str = std::get_if<std::string>(&name_param)) {
        source_name = *str;
    }

    auto url_param = event.get_parameter("url");
    if (auto* str = std::get_if<std::string>(&url_param)) {
        source_url = *str;
    }

    if (source_name.empty() || source_url.empty()) {
        event.edit_response("Please provide both a name and a URL.");
        return;
    }

    if (source_url.rfind("http://", 0) != 0 && source_url.rfind("https://", 0) != 0) {
        event.edit_response("The URL must start with http:// or https://");
        return;
    }
    if (source_url.rfind("http://", 0) != 0 && source_url.rfind("https://", 0) != 0) {
        event.edit_response("The URL must start with http:// or https://");
        return;
    }

    auto safety = UrlSafety::validate(source_url);
    if (!safety.ok) {
        event.edit_response("URL denied: " + safety.reason);
        return;
    }

    BotClient& client = static_cast<BotClient&>(bot);
    sqlite3* db = client.getDB();

    std::string existing_name;
    if (KnowledgeCache::findSourceByUrl(db, guild_id, source_url, existing_name)) {
        event.edit_response("This URL is already saved under the name `" + existing_name + "`.");
        return;
    }

    bool name_exists = KnowledgeCache::sourceNameExists(db, guild_id, source_name);

    bool saved = name_exists
        ? KnowledgeCache::updateSourceUrl(db, guild_id, source_name, source_url)
        : KnowledgeCache::addSource(db, guild_id, source_name, source_url);

    if (saved) {
        client.knowledgeCache.reloadGuild(guild_id, db);

        if (name_exists) {
            event.edit_response("Knowledge source `" + source_name + "` successfully updated with the new URL.");
        } else {
            event.edit_response("Knowledge source `" + source_name + "` successfully added and loaded.");
        }
    } else {
        event.edit_response("Failed to save the knowledge source to the database.");
    }
}