#include "ScamMessageScan.h"


bool ScamMessageScan::ScanMessage(dpp::cluster& bot, dpp::snowflake msgID, dpp::snowflake userID, dpp::snowflake channelID, int imageCount, time_t timestamp, std::vector<dpp::attachment> attachments)
{
    dpp::snowflake bot_userID = 1528010936210817135;
    std::vector<ImagePost>& posts = userPosts[userID];
    if (userID == bot_userID)
    {
        return false;
    }
    for (int i = posts.size() - 1; i >= 0; i--)
    {
        if (timestamp - posts[i].timestamp > 5)
        {
            posts.erase(posts.begin() + i);
        }
    }
    bool isScam = false;
    bool skipCheck = false;

    for (const auto& attachment : attachments)
    {


        if (!attachment.filename.ends_with(".png") && !attachment.filename.ends_with(".jpg") && !attachment.filename.ends_with(".gif"))
        {
            isScam = false;
            skipCheck = true;
            break;
        };
    }
    if (!skipCheck)
    {
        for (const ImagePost& post : posts)
        {
            if (post.channelID != channelID && post.imageCount == imageCount)
            {
                isScam = true;
                bot.message_delete(post.msgID, post.channelID);
            }
        }
    }


    ImagePost newPost;
    newPost.msgID = msgID;
    newPost.timestamp = timestamp;
    newPost.imageCount = imageCount;
    newPost.channelID = channelID;
    posts.push_back(newPost);

    return isScam;
}
