#include "ClickHandler.h"

void ClickHandler::onButtonClick(const dpp::button_click_t& event)
{
    std::string custom_id = event.custom_id;
    dpp::snowflake guild_id = event.command.guild_id;
    dpp::guild* g = dpp::find_guild(guild_id);

    if (!g) {
        event.reply(dpp::message("Internal error: guild not found.").set_flags(dpp::m_ephemeral));
        return;
    }

    uint32_t ban_message_delete_time = 3600; // 1 hour

    size_t sep = custom_id.find(':');
    if (sep == std::string::npos) return;

    std::string action = custom_id.substr(0, sep);
    dpp::snowflake thread_id = std::stoull(custom_id.substr(sep + 1));
    dpp::snowflake userID = thread_id; //custom id is used for threads aswell as user ids, depending on the button

    dpp::embed embed = dpp::embed();
    dpp::message msg;
    msg.set_channel_id(thread_id);
    dpp::component row;

    if (action == "info_ask_button")
    {
        embed.set_color(dpp::colors::blurple)
        .set_title("Information Request")
        .set_description("Please send more information about the problem like mods list and other info that might help to find the cause");
    }
    else if (action == "log_ask_button")
    {
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
    }

    else if (action == "log_helper_button")
    {
        embed.set_color(dpp::colors::blurple)
        .set_title("Game Log Request")
        .set_description("You can find your logs here: ``` YourMinecraftDirectory/logs/latest.log``` Or you can select your launcher here to get a Detailed description:");

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
    }

    else if (action == "issue_note_button")
    {
        embed.set_color(dpp::colors::green)
        .set_title("Issue Noted!")
        .set_description("A dev has noted this issue, it will be fixed as soon as possible");

    }
    else if (action == "softban_button")
    {
        dpp::permission perms = g->base_permissions(event.command.member);

        if (!(perms & dpp::p_ban_members))
        {
            event.reply(dpp::message("You are not allowed to ban members!").set_flags(dpp::m_ephemeral));
            return;
        }

        bot.guild_ban_add(guild_id, userID, ban_message_delete_time);

        bot.start_timer([this, guild_id, userID](dpp::timer handle) {
            bot.guild_ban_delete(guild_id, userID);
            bot.stop_timer(handle);
        }, 10);

        event.reply(dpp::message("Softban executed.").set_flags(dpp::m_ephemeral));
        return;
    }

    msg.add_embed(embed);
    bot.message_create(msg);

    event.reply(dpp::message("Done!").set_flags(dpp::m_ephemeral));
}

void ClickHandler::onSelectClick(const dpp::select_click_t& event)
{
    std::string select_id = event.custom_id;
    std::string selected_option_id = event.values[0];
    dpp::embed embed;
    dpp::message msg;

    if (select_id.empty()) return;

    else if (select_id == "launcher_select_menu")
    {
        if (selected_option_id == "prism_launcher")
        {
            embed.set_title("Prism Launcher")
                .set_description(R"(**1.** Open the Prism Launcher
                                    **2.** Right-click on your modpack and click **Edit**.
                                    **3.** Press **Copy**.)");
        }
        else if (selected_option_id == "curse_forge")
        {

            embed.set_title("Curse Forge")
                .set_description(R"(**1.** Open the Curse Forge App
                                    **2.** Open on your modpack and click the 3 dots next to the **Play** button
                                    **3.** Click **Open Folder** and go into the logs folder
                                    **4.** Copy the **latest.log** file)");

        }
        else if (selected_option_id == "modrinth")
        {
            embed.set_title("Modrinth App")
                .set_description(R"(**1.** Open the Modrinth App
                                    **2.** In the instances list, click your instance.
                                    **3.** Navigate to the **Logs** tab.
                                    **4.** Select the **latest.log** file the dropdown list.
                                    **5.** Click **Copy** )");
        }
        else if (selected_option_id == "atlauncher")
        {
            embed.set_title("Atlauncher")
                .set_description(R"(**1.** Open the AtLauncher
                                    **2.** Click on the **Instances** Tab
                                    **3.** Locate your Instance and click **Open Folder**.
                                    **4.** Open the **logs** folder and right-click the **latest.log** file.
                                    **5.** Press **Copy** )");
        }
        else if (selected_option_id == "minecraft_launcher")
        {
            embed.set_title("Minecraft Launcher")
                .set_description(R"(**1.** Press Windows + R
                                    **2.** Type %appdata%
                                    **3.** Open .minecraft
                                    **4.** Open the **logs** folder and locate latest.log
                                    **5.** Right-Click the file and press **Copy**)");
        }

        msg.add_embed(embed);
        msg.set_flags(dpp::m_ephemeral);
        event.reply(dpp::message(msg));

    }

}