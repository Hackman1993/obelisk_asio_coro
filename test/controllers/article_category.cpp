//
// Created by Hackman.Lo on 2024/3/1.
//

#include "article_category.h"

#include <regex>

#include "controller_base.h"
#include "database/database.h"
#include "database/mongo/mongo_connection.h"
#include "http/response/json_response.h"
#include "http/validator/required_validator.h"
#include <boost/spirit/home/x3.hpp>

#include "../common/user_data.h"
#include "../storage/storage.h"
#include "http/exception/http_exception.h"
using namespace obelisk::http;
using namespace obelisk::database;
using namespace obelisk::http::validator;
using namespace bsoncxx::builder::basic;
using namespace boost::spirit::x3;

boost::cobalt::task<std::unique_ptr<http_response>> get_article_list(http_request_wrapper&request) {
    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article"];
    document filter;
    mongocxx::options::find option{};
    option.projection(make_document(
        kvp("_id", true),
        kvp("abstract", true),
        kvp("author", true)
    ));

    auto result = co_await paginate(collection, request, filter, option);

    co_return std::make_unique<json_response>(result, EST_OK);
}

std::vector<std::string> get_oss_attachment_keys(const std::string&html_data) {
    std::string content = html_data;
    std::vector<std::string> attachment_keys;
    const std::string oss_url = "https://hl-blog-bucket.oss-cn-chengdu.aliyuncs.com/";
    const std::regex reg("(src=\\\"" + oss_url + R"(([^\"]*)\")|(src=')" + oss_url + "([^\']*)')");
    std::smatch match;
    while (std::regex_search(content, match, reg)) {
        std::string data{};
        if (match[2].matched)
            data = match[2];
        else if (match[4].matched)
            data = match[4];
        attachment_keys.emplace_back(data);
        //obelisk::storage::storage_manager::get("alioss").delete_tag(data);
        content = match.suffix();
    }
    return attachment_keys;
}

boost::cobalt::task<void> sync_attachment_data(const std::vector<std::string>&attachments, const bsoncxx::oid&oid) {
    auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_attachment"];
    sahara::container::unordered_smap_u<bool> attachment_urls;
    array array;
    for (const auto&item_key: attachments) {
        array.append(item_key);
        attachment_urls.emplace(item_key, true);
    }

    bsoncxx::builder::basic::array oid_needs_to_update;
    mongocxx::options::find find_option;
    bsoncxx::types::b_date date(std::chrono::system_clock::now());
    find_option.projection(make_document(kvp("_id", true), kvp("url", true)));
    auto result = collection.find(make_document(
        kvp("url", make_document(kvp("$in", array)))
    ));
    for (auto&item: result) {
        if (attachment_urls.contains(std::string(item["url"].get_string().value))) {
            oid_needs_to_update.append(item["_id"].get_oid());
            attachment_urls.erase(std::string(item["url"].get_string().value));
        }
    }

    bsoncxx::builder::basic::array obj_needs_to_inserted;
    for (const auto&k: attachment_urls) {
        obj_needs_to_inserted.append(make_document(
            kvp("url", k.first),
            kvp("provider", "alioss"),
            kvp("deleted_at",date)
        ));
    }


        collection.count_documents(make_document());
    co_return;
}

boost::cobalt::task<std::unique_ptr<http_response>> create_article(http_request_wrapper&request) {
    co_await request.validate({
        {"title", {required()}},
        {"content", {required()}},
        {"abstract", {required()}},
    });
    const auto&udata = std::any_cast<user_data>(request.additional_data()["__user_info"]);
    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    std::string content(request.params()["content"].as_string());
    const auto attachments = get_oss_attachment_keys(content);
    for (const auto&attachment: attachments) {
        obelisk::storage::storage_manager::get("alioss").delete_tag(attachment);
    }

    bsoncxx::oid oid(udata.user_id);
    auto collection = (*conn)["hl_blog_database"]["c_article"];
    const auto result = collection.insert_one(make_document(
        kvp("title", request.params()["title"].as_string()),
        kvp("content", request.params()["content"].as_string()),
        kvp("abstract", request.params()["abstract"].as_string()),
        kvp("author", oid)
    ));
    if (!result.has_value())
        throw http_exception("error.database.mongo.execution_error", EST_INTERNAL_SERVER_ERROR);

    co_return std::make_unique<json_response>(boost::json::object{
        {
            "data", {
                {"id", result.value().inserted_id().get_oid().value.to_string()},
                {"title", request.params()["title"].as_string()},
                {"abstract", request.params()["abstract"].as_string()}
            }
        }
    });
}


boost::cobalt::task<std::unique_ptr<http_response>> update_article(http_request_wrapper&request) {
    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");

    const std::string content(request.params()["content"].as_string());
    auto attachments = get_oss_attachment_keys(content);
    for (const auto&attachment: attachments) {
        obelisk::storage::storage_manager::get("alioss").delete_tag(attachment);
    }
    co_return nullptr;
}
