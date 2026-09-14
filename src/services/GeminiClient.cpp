#include "GeminiClient.h"
#include "nlohmann/json.hpp"
#include "cpr/cpr.h"

using json = nlohmann::json;


GeminiClient::GeminiClient(dpp::cluster& bot, std::string api_key)
    : m_bot(bot), m_api_key(std::move(api_key))
{
    if (m_api_key.empty()) {
        std::cerr << "API Key is empty" << std::endl;
    }
}
void GeminiClient::generate_text(const std::string& prompt, std::function<void(std::string)> callback) {

    std::string url = "https://api.groq.com/openai/v1/chat/completions";
    std::string model = std::getenv("AI_MODEL");

    json body = {
        {"model", model},
        {"messages", json::array({
            {
                {"role", "user"},
                {"content", prompt}
            }
        })},
        {"top_p", 1},
        {"stream", false},
        {"reasoning_effort", "medium"},
        {"tools", json::array({
            {
                {"type", "browser_search"}
            }
        })}
    };


    m_bot.request(
        url,
        dpp::m_post,
        [callback](const dpp::http_request_completion_t& response) {
            if (response.status == 200) {
                try {
                    auto res_json = json::parse(response.body);

                    std::string ai_text = res_json["choices"][0]["message"]["content"].get<std::string>();
                    callback(ai_text);
                } catch (...) {
                    callback("Error while Parsing Ai-Answer");
                }
            } else {
                callback("API-Error (Status: " + std::to_string(response.status) + ")");
            }
        },
        body.dump(),
        "application/json",
        {{"Authorization", "Bearer " + m_api_key}}
    );
}

void GeminiClient::summarize_post(const std::string& title, const std::string& content, std::function<void(std::string)> callback) {
    std::string prompt = "You Are a Discord-Moderation-Bot. Summarize following Article. Write as little as possible without but enough to understand the original post."
                         "if you think not enough info is provided nicely say that. You are not replying to the user but to the developers\n\n"
                         "Titel: " + title + "\n"
                         "Content: " + content;

    generate_text(prompt, callback);
}

void GeminiClient::answer_faq(const std::string& user_question, const std::string& faq_data, std::function<void(std::string)> callback) {


    std::string prompt = std::string(R"(You are a support assistant that answers user questions using ONLY the FAQ data provided below.

            Rules:
            1. Search FAQ for relevant info.
            2. If found: quote verbatim, no paraphrasing/summarizing/added words.
            3. If not found OR unsure whether an answer exists: respond exactly )") + noInfo_answer + R"(
            4. No commentary, opinions, greetings, or explanations.
            5. Tone: neutral, factual; no emotional language, exclamation marks, or filler.
            6. If a Minecraft log is posted (Discord link or site linking to one): review log, identify cause, check FAQ for a possible fix.

            --- FAQ DATA START ---
            )" + faq_data + "\n--- FAQ DATA END ---\n\n--- USER QUESTION START ---\n" + user_question + "\n--- USER QUESTION END ---";

    generate_text(prompt, callback);
}

void GeminiClient::custom_request(const std::string& request, std::function<void(std::string)> callback)
{
    std::string prompt = "You are a discord bot used by admins"
                         "Your job is to do whatever the admins tell you to do, always be friendly and if you are not sure about your answer clearly say that! "
                         "This is the command for you:\n"
                         + request + "\n";

    generate_text(prompt, callback);
}

std::string GeminiClient::fetch_website_info(const std::string& url)
{
    cpr::Response r = cpr::Get(cpr::Url{url});
    std::string content;

    if (r.status_code == 200)
    {
        std::cout << "Successfully loaded\n\n";
        content = r.text;

    }
    else
    {
        std::cerr << "Error while loading the website: " << r.status_code << std::endl;
    }

    return content;
}
