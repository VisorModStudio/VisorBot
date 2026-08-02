#pragma once
#include "BotClient.h"
#include <unordered_set>

class EventModule
{
    public:
    virtual ~EventModule() = default;

    virtual void registerHandlers() = 0;
    std::unordered_set<dpp::snowflake> processed_threads;


};


