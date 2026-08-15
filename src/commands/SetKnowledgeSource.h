#pragma once

#include "../src/Command.h"
#include "../services/KnowledgeCache.h"
#include <dpp/dpp.h>
#include <string>
#include "../src/BotClient.h"

class SetKnowledgeSource : public Command {
private:
    KnowledgeCache& knowledgeCache;

public:
    SetKnowledgeSource(BotClient& bot_ref, KnowledgeCache& knowledgeCache_ref)
        : Command(bot_ref), knowledgeCache(knowledgeCache_ref) {}

    std::string getName() const override;
    std::string getDescription() const override;
    void execute(const dpp::slashcommand_t& event) override;
};
