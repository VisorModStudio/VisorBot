#pragma once
#include "BotClient.h"


class EventModule
{
    public:
    virtual ~EventModule() = default;

    virtual void registerHandlers() = 0;
};


