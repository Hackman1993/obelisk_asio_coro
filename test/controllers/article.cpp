//
// Created by Hackman.Lo on 2024/3/1.
//

#include "article.h"

#include <regex>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "controller_base.h"
#include "database/database.h"
#include "database/mongo/mongo_connection.h"
#include "http/response/json_response.h"
#include "http/validator/required_validator.h"
#include <boost/spirit/home/x3.hpp>

#include "attachment_controller.h"
#include "../common/user_data.h"
#include "../storage/storage.h"
#include "http/exception/http_exception.h"
using namespace obelisk::http;
using namespace obelisk::database;
using namespace obelisk::http::validator;
using namespace bsoncxx::builder::basic;

boost::cobalt::task<std::unique_ptr<http_response>> article_controller::get_article_list(http_request_wrapper&request) {
    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article"];
    auto filter = make_document(kvp("$or", make_array(
                                        make_document(kvp("deleted_at", bsoncxx::types::b_null{})),
                                        make_document(kvp("deleted_at",
                                                          make_document(kvp(
                                                              "$gt", bsoncxx::types::b_date(
                                                                  std::chrono::system_clock::now())))))
                                    )));

    auto result = co_await paginate(collection, request, filter, make_array(
                                        make_document(kvp("$addFields",
                                                          make_document(
                                                              kvp("id", make_document(kvp("$toString", "$_id")))))),
                                        make_document(kvp("$lookup", make_document(
                                                              kvp("from", "c_sys_admin"),
                                                              kvp("localField", "author"),
                                                              kvp("foreignField", "_id"),
                                                              kvp("as", "author"),
                                                              kvp("pipeline", make_array(make_document(kvp(
                                                                      "$project", make_document(
                                                                          kvp("_id", 0),
                                                                          kvp("password", 0)
                                                                      )))))
                                                          ))),
                                        make_document(kvp("$project", make_document(
                                                              kvp("updated_at",
                                                                  make_document(kvp(
                                                                      "$dateToString",
                                                                      make_document(
                                                                          kvp("format", "%Y-%m-%d %H:%M:%S"),
                                                                          kvp("date", "$updated_at"))))),
                                                              kvp("_id", false),
                                                              kvp("id", true),
                                                              kvp("abstract", true),
                                                              kvp("title", true),
                                                              kvp("categories", true),
                                                              kvp("cover", true)
                                                          ))),
                                        make_document(kvp("$sort", make_document(
                                                              kvp("updated_at", -1)
                                                          )))
                                    ));

    co_return std::make_unique<json_response>(result, EST_OK);
}

boost::cobalt::task<std::unique_ptr<http_response>>
article_controller::get_article_detail(http_request_wrapper&request) {
    co_await request.validate({{"article_id", {required()}}});
    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article"];


    mongocxx::pipeline pipeline;
    pipeline.append_stage(make_document(
        kvp("$match", make_document(kvp("_id", bsoncxx::oid(request.params()["article_id"].as_string()))))));
    pipeline.append_stage(make_document(kvp("$addFields",
                                            make_document(kvp("id", make_document(kvp("$toString", "$_id")))))));
    pipeline.append_stage(make_document(kvp("$project", make_document(
                                                kvp("_id", false),
                                                kvp("id", true),
                                                kvp("title", true),
                                                kvp("content", true),
                                                kvp("categories", true),
                                                kvp("abstract", true),
                                                kvp("cover", true)

                                            ))));
    auto data = collection.aggregate(pipeline);
    if (data.begin() == data.end())
        throw http_exception("error.article.not_found", EST_NOT_FOUND);

    co_return std::make_unique<json_response>(make_document(kvp("data", *data.begin())), EST_OK);
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
        content = match.suffix();
    }
    return attachment_keys;
}

boost::cobalt::task<void> sync_attachment_data(const std::vector<std::string>&attachments, const bsoncxx::oid&oid) {
    auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_attachment"];
    sahara::container::unordered_smap_u<bool> attachment_urls;
    array attachment_url_array;
    for (const auto&item_key: attachments) {
        attachment_url_array.append(item_key);
        attachment_urls.emplace(item_key, true);
    }

    array oid_needs_to_update;
    array oid_needs_to_delete;
    mongocxx::options::find find_option;
    find_option.projection(make_document(kvp("_id", true), kvp("url", true)));

    auto result = collection.find(
        make_document(
            kvp("$or", make_array(
                    make_document(kvp("url", make_document(kvp("$in", attachment_url_array)))),
                    make_document(kvp("reference", make_document(kvp("$eq", oid))))
                ))
        ), find_option);
    for (auto&item: result) {
        if (attachment_urls.contains(std::string(item["url"].get_string().value))) {
            oid_needs_to_update.append(item["_id"].get_oid());
        }
        else {
            oid_needs_to_delete.append(item["_id"].get_oid());
        }
    }

    mongocxx::options::update update_option;
    collection.update_many(
        make_document(kvp("_id", make_document(kvp("$in", oid_needs_to_update)))),
        make_document(
            kvp("$addToSet", make_document(kvp("reference", oid))),
            kvp("$set", make_document(kvp("deleted_at", bsoncxx::types::b_null{})))
        ), update_option);
    collection.update_many(
        make_document(kvp("_id", make_document(kvp("$in", oid_needs_to_delete)))),
        make_document(kvp("$pull", make_document(kvp("reference", oid)))), update_option);


    auto object_to_delete = collection.find(make_document(kvp("reference", make_document(kvp("$eq", make_array())))));
    array deleted_object_ids;
    for (const auto&item: object_to_delete) {
        deleted_object_ids.append(item["_id"].get_oid());
        obelisk::storage::storage_manager::get("alioss").delete_tag(std::string(item["url"].get_string().value));
    }
    collection.delete_many(make_document(kvp("_id", make_document(kvp("$in", deleted_object_ids)))));
    co_return;
}

boost::cobalt::task<std::unique_ptr<http_response>> article_controller::create_article(http_request_wrapper&request) {
    co_await request.validate({
        {"title", {required()}},
        {"content", {required()}},
        {"abstract", {required()}}
    });
    const auto&udata = std::any_cast<user_data>(request.additional_data()["__user_info"]);
    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    std::string content(request.params()["content"].as_string());
    auto attachments = get_oss_attachment_keys(content);

    std::optional<std::string> cover_url;
    if(request.filebag().contains("cover")) {
        auto cover = request.filebag()["cover"];
        cover_url = co_await save_attachment(cover, bsoncxx::oid(udata.user_id));
        attachments.push_back(boost::algorithm::replace_first_copy(cover_url.value(), "${OSS_URL}/", ""));
    }
    bsoncxx::oid oid(udata.user_id);
    auto collection = (*conn)["hl_blog_database"]["c_article"];
    array categories;
    if (request.params().contains("categories") && request.params()["categories"].is_array()) {
        for (const auto&item: request.params()["categories"].as_array()) {
            categories.append(item.as_string());
        }
    }

    document document;
    document.append(kvp("title", request.params()["title"].as_string()));
    document.append(kvp("content", request.params()["content"].as_string()));
    document.append(kvp("abstract", request.params()["abstract"].as_string()));
    document.append(kvp("author", oid));
    document.append( kvp("categories", categories));
    document.append(kvp("created_at", bsoncxx::types::b_date(std::chrono::system_clock::now())));
    document.append(kvp("updated_at", bsoncxx::types::b_date(std::chrono::system_clock::now())));
    if (cover_url.has_value())
        document.append(kvp("cover", "${OSS_URL}/" + cover_url.value()));

    const auto result = collection.insert_one(document.view());
    if (!result.has_value())
        throw http_exception("error.database.mongo.execution_error", EST_INTERNAL_SERVER_ERROR);
    co_await sync_attachment_data(attachments, result.value().inserted_id().get_oid().value);

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


boost::cobalt::task<std::unique_ptr<http_response>> article_controller::update_article(http_request_wrapper&request) {
    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article"];
    co_await request.validate({{"article_id", {required()}}});

    bsoncxx::oid oid(request.params()["article_id"].as_string());
    auto origin_data = collection.find_one(make_document(kvp("_id", oid))).value();
    std::string content_data(origin_data["content"].get_string());

    const auto&udata = std::any_cast<user_data>(request.additional_data()["__user_info"]);

    document document{};
    if (request.params().contains("content")) {
        content_data = request.params()["content"].as_string();
        document.append(kvp("content", content_data));
    }
    std::vector<std::string> attachments = get_oss_attachment_keys(content_data);

    if (request.params().contains("title")) {
        document.append(kvp("title", request.params()["title"].as_string()));
    }
    if (request.params().contains("abstract")) {
        document.append(kvp("abstract", request.params()["abstract"].as_string()));
    }
    if(request.filebag().contains("cover")) {
        auto cover_url = co_await save_attachment(request.filebag()["cover"], bsoncxx::oid(udata.user_id));
        attachments.push_back(boost::algorithm::replace_first_copy(cover_url, "${OSS_URL}/", ""));
        document.append(kvp("cover", "${OSS_URL}/" + cover_url));

    }else if(!request.params().contains("cover")) {
        document.append(kvp("cover", bsoncxx::types::b_null{}));
    }

    array categories;
    if (request.params().contains("categories") && request.params()["categories"].is_array()) {
        for (const auto&item: request.params()["categories"].as_array()) {
            categories.append(item.as_string());
        }
    }

    document.append(kvp("categories", categories));
    document.append(kvp("updated_at", bsoncxx::types::b_date(std::chrono::system_clock::now())));
    collection.update_one(make_document(kvp("_id", oid)), make_document(kvp("$set", document)));
    co_await sync_attachment_data(attachments, oid);

    co_return std::make_unique<json_response>(boost::json::object{
        {
            "data", {
                {"id", oid.to_string()}
            }
        }
    });
}

boost::cobalt::task<std::unique_ptr<http_response>> article_controller::delete_article(http_request_wrapper&request) {
    co_await request.validate({{"article_id", {required()}}});
    auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article"];
    collection.update_one(make_document(kvp("_id", bsoncxx::oid(request.params()["article_id"].as_string()))),
                          make_document(kvp("$set", make_document(
                                                kvp("deleted_at",
                                                    bsoncxx::types::b_date(std::chrono::system_clock::now()))))));
    co_return std::make_unique<json_response>(boost::json::object{
        {
            "data", {
                {"id", request.params()["article_id"].as_string()}
            }
        }
    });
}
