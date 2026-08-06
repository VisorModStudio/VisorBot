#pragma once
#include <string>
#include "dpp/dpp.h"
#include <functional>


class GeminiClient
{
    public:
    GeminiClient(dpp::cluster& bot, std::string api_key);

    void generate_text(const std::string& prompt, std::function<void(std::string)> callback);

    void summarize_post(const std::string& title, const std::string& content, std::function<void(std::string)> callback);
    void answer_faq(const std::string& user_question, const std::string& faq_data, std::function<void(std::string)> callback);
    void custom_request(const std::string& request,std::function<void(std::string)> callback);


    private:
    dpp::cluster& m_bot;
    std::string m_api_key;
    std::string m_model;
};



