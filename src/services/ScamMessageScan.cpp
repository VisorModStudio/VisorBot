#include "ScamMessageScan.h"


std::string ScamMessageScan::ScanMessage(dpp::cluster& bot, dpp::snowflake msgID, dpp::snowflake userID, dpp::snowflake channelID, int imageCount, time_t timestamp, std::vector<dpp::attachment> attachments)
{
    dpp::snowflake bot_userID = 1528010936210817135;
    std::vector<ImagePost>& posts = userPosts[userID];
    static std::string blacklistMsgID;
    static std::string blacklistUserID;
    //static blacklisted 1. message id(doenst get deleted), static black listed user
    //todo make if user Scanned as scammer -> ignore further

    if (userID == bot_userID)
    {
        return "0";
    }
    for (int i = posts.size() - 1; i >= 0; i--)
    {
        if (timestamp - posts[i].timestamp > 5)
        {
            posts.erase(posts.begin() + i);
        }
    }
    std::string scamMsgChannel = "0";
    bool skipCheck = false;

    for (const auto& attachment : attachments)
    {


        if (!attachment.filename.ends_with(".png") && !attachment.filename.ends_with(".jpg") && !attachment.filename.ends_with(".gif"))
        {
            scamMsgChannel = "0";
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


                if (post.msgID.str() != blacklistMsgID)
                {
                    bot.message_delete(msgID, channelID);

                }else
                {
                    bot.message_delete(post.msgID, post.channelID);
                }



                if (blacklistUserID != userID.str())
                {
                    scamMsgChannel = post.channelID.str();
                }


                blacklistUserID = userID.str();
                blacklistMsgID = msgID.str();


            }
        }
    }



    ImagePost newPost;
    newPost.msgID = msgID;
    newPost.timestamp = timestamp;
    newPost.imageCount = imageCount;
    newPost.channelID = channelID;
    posts.push_back(newPost);



    return scamMsgChannel;
}
