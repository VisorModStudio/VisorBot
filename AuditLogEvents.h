#pragma once

enum class AuditLogEvent
{
    ChannelCreate = 10,
    ChannelUpdate = 11,
    ChannelDelete = 12,
    ChannelOverwriteCreate = 13, //TODO
    ChannelOverwriteUpdate = 14, //TODO
    ChannelOverwriteDelete = 15, //TODO

    MemberKick = 20,
    MemberPrune = 21, //leave out?
    MemberBanAdd = 22,
    MemberBanRemove = 23,
    MemberUpdate = 24,
    MemberRoleUpdate = 25,
    MemberMove = 26,
    MemberDisconnect = 27,
    BotAdd = 28,

    RoleCreate = 30,
    RoleUpdate = 31,
    RoleDelete = 32,

    InviteCreate = 40,
    InviteUpdate = 41,//leave out?
    InviteDelete = 42,

    WebhookCreate = 50,
    WebhookUpdate = 51,
    WebhookDelete = 52,

    EmojiCreate = 60,
    EmojiUpdate = 61,
    EmojiDelete = 62,

    MessageDelete = 72,
    MessageBulkDelete = 73,
    MessagePin = 74,
    MessageUnpin = 75,

    IntegrationCreate = 80,
    IntegrationUpdate = 81,
    IntegrationDelete = 82,

    StickerCreate = 90,
    StickerUpdate = 91,
    StickerDelete = 92,

    ScheduledEventCreate = 100,
    ScheduledEventUpdate = 101,
    ScheduledEventDelete = 102,

    ThreadCreate = 110,
    ThreadUpdate = 111,
    ThreadDelete = 112,

    AutoModRuleCreate = 140,
    AutoModRuleUpdate = 141,
    AutoModRuleDelete = 142,
    AutoModBlockMessage = 143,
    AutoModFlagToChannel = 144,
    AutoModUserCommunicationDisabled = 145,
    AutoModQuarantineUser = 146,
};