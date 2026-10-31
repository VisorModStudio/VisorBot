#include "dpp/dpp.h"

#include "GameLogHelperCommand.h"

std::string GameLogHelperCommand::getName() const
{
    return "gameloghelper";
}
std::string GameLogHelperCommand::getDescription() const
{
    return "Panel to help find game logs for specific launchers";
}

void GameLogHelperCommand::execute(const dpp::slashcommand_t& command)
{
    dpp::embed embed = dpp::embed();
    dpp::message msg;
    dpp::component row;


    embed.set_color(dpp::colors::blurple)
        .set_title("Game Log Request")
        .set_description("Please Post your Game Log! You can find it here: ``` YourMinecraftDirectory/logs/latest.log``` Or you can select your launcher here to get a Detailed description:");

    dpp::component launcher_select_menu;

    launcher_select_menu.set_type(dpp::cot_selectmenu)
                        .set_id("launcher_select_menu")
                        .set_placeholder("please select your launcher");

    launcher_select_menu.add_select_option(dpp::select_option("Prism Launcher", "prism_launcher", ""))
                        .add_select_option(dpp::select_option("Curse Forge", "curse_forge", ""))
                        .add_select_option(dpp::select_option("Modrinth App", "modrinth", ""))
                        .add_select_option(dpp::select_option("ATlauncher", "atlauncher", ""))
                        .add_select_option(dpp::select_option("Minecraft Launcher", "minecraft_launcher", ""));

    row.add_component(launcher_select_menu);
    msg.add_component(row);

    msg.add_embed(embed);


    command.reply(msg);



}