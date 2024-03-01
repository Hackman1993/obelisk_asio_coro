//
// Created by Hackman.Lo on 2024/3/1.
//

#include "article_category.h"

#include "controller_base.h"
#include "database/database.h"
#include "database/mongo/mongo_connection.h"
#include "http/response/json_response.h"

using namespace obelisk::http;
using namespace obelisk::database;
using namespace obelisk::http::validator;

boost::cobalt::task<std::unique_ptr<http_response>> create_article(http_request_wrapper&request) {
    auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article"];
    bsoncxx::builder::basic::document filter;

    auto result = paginate(collection, request, filter);

    co_return std::make_unique<json_response>(result, EST_OK);
    co_return nullptr;
}
