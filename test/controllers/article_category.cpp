//
// Created by Hackman.Lo on 2024/3/18.
//

#include "article_category.h"

#include <obelisk/database/connection_pool.h>
#include <obelisk/http/response/json_response.h>
#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/validator/required_validator.h>
using namespace obelisk::http;
using namespace obelisk::database;
using namespace obelisk::http::validator;
obelisk::task<std::unique_ptr<http_response>> article_category::get_article_category_list(http_request_wrapper& request) {



    co_return nullptr;
}

obelisk::task<std::unique_ptr<http_response>> article_category::get_article_category_full(http_request_wrapper& request) {

    co_return nullptr;
}

obelisk::task<std::unique_ptr<http_response>> article_category::create_article_category(http_request_wrapper& request) {
    co_return nullptr;
}

obelisk::task<std::unique_ptr<http_response>> article_category::update_article_category(http_request_wrapper& request) {
    co_return nullptr;
}

obelisk::task<std::unique_ptr<obelisk::http::http_response>> article_category::delete_article_category(obelisk::http::http_request_wrapper& request) {
    co_return nullptr;
}
