#include "GeminiClient.h"
#include "nlohmann/json.hpp"

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


    json body = {
        {"model", "llama-3.3-70b-versatile"},
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
    std::string prompt = "You are a helpful support assistant.\n"
                         "Use ONLY the following FAQ knowledge to answer the user's question. "
                         "If the answer is not in the FAQ, politely reply that you do not know and that a moderator will assist.\n\n"
                         "FAQ Data:\n" + faq_data + "\n\n"
                         "User Question: " + user_question;

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
