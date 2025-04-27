//
// Created by Hackman.Lo on 2024/3/1.
//

#include "article.h"

#include <regex>
#include "obelisk/database/database.h"
#include "obelisk/http/response/json_response.h"
#include "obelisk/http/validator/required_validator.h"
using namespace obelisk::http;
using namespace obelisk::database;
using namespace obelisk::http::validator;

namespace controller
{
    obelisk::task<std::unique_ptr<http_response>> article_controller::get_article_list(http_request_wrapper& request)
    {
        co_return nullptr;
    }

    obelisk::task<std::unique_ptr<http_response>>
    article_controller::get_article_detail(http_request_wrapper& request)
    {
        co_return nullptr;
    }

    std::vector<std::string> get_oss_attachment_keys(const std::string& html_data)
    {
        std::string content = html_data;
        std::vector<std::string> attachment_keys;
        const std::string oss_url = "https://hl-blog-bucket.oss-cn-chengdu.aliyuncs.com/";
        const std::regex reg("(src=\\\"" + oss_url + R"(([^\"]*)\")|(src=')" + oss_url + "([^\']*)')");
        std::smatch match;
        while (std::regex_search(content, match, reg))
        {
            std::string data{};
            if (match[2].matched)
                data = match[2];
            else if (match[4].matched)
                data = match[4];
            attachment_keys.emplace_back(data);
            content = match.suffix();
        }
        return attachment_keys;
    }

    obelisk::task<std::unique_ptr<http_response>> article_controller::create_article(http_request_wrapper& request)
    {
        co_return nullptr;
    }


    obelisk::task<std::unique_ptr<http_response>> article_controller::update_article(http_request_wrapper& request)
    {
        co_return nullptr;
    }

    obelisk::task<std::unique_ptr<http_response>> article_controller::delete_article(http_request_wrapper& request)
    {
        co_return nullptr;
    }
}
