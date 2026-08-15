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
            1. Search the FAQ data for information relevant to the user's question.
            2. If relevant information exists, quote it verbatim (word-for-word) from the source. Do not paraphrase, summarize, or add any words of your own.
            3. If no relevant information exists in the FAQ data, respond with exactly: "This information is not available in the FAQ. A staff member will review your question."
            4. Never add commentary, opinions, greetings, or explanations of your own.
            5. Respond in a neutral, factual tone. Do not use emotional language, exclamation marks, or conversational filler.

            --- FAQ DATA START ---
            )") + faq_data + "\n--- FAQ DATA END ---\n\n--- USER QUESTION START ---\n" + user_question + "\n--- USER QUESTION END ---";

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
