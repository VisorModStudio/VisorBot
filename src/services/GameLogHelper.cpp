#include "GameLogHelper.h"




dpp::message GameLogHelper::getLogHelpMsg(dpp::snowflake channelID)
{
    dpp::embed embed = dpp::embed()
                    .set_color(dpp::colors::blurple)
                    .set_title("Helper")
                    .set_description("Do you need help finding your game logs? If so please click the button below!");

    dpp::message msg(channelID, embed);

    msg.add_component(
        dpp::component().add_component(
            dpp::component()
                .set_label("Log Helper")
                .set_type(dpp::cot_button)
                .set_emoji("❓")
                .set_style(dpp::cos_primary)
                .set_id("log_helper_button:" + channelID.str())));

    return msg;
};

void GameLogHelper::sendLogHelpMsg(const dpp::message& msg)
{
    bot.message_create(msg);
}
