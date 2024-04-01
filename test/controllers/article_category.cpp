//
// Created by Hackman.Lo on 2024/3/18.
//

#include "article_category.h"

#include "database/connection_pool.h"
#include "http/response/json_response.h"
#include "http/exception/http_exception.h"
#include "database/mongo/mongo_connection.h"
#include "http/validator/required_validator.h"
using namespace obelisk::http;
using namespace obelisk::database;
using namespace obelisk::http::validator;
using namespace bsoncxx::builder::basic;
boost::cobalt::task<std::unique_ptr<http_response>> article_category::get_article_category_list(http_request_wrapper& request) {

    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article_category"];
    auto filter = make_document(kvp("$or", make_array(
        make_document(kvp("deleted_at", bsoncxx::types::b_null{})),
        make_document(kvp("deleted_at", make_document(kvp("$gt", bsoncxx::types::b_date(std::chrono::system_clock::now())))))
    )));

    auto result = co_await paginate(collection, request, filter, make_array(
        make_document(kvp("$addFields", make_document(kvp("id", make_document(kvp("$toString", "$_id")))))),
        make_document(kvp("$project", make_document(
            kvp("updated_at", make_document(kvp("$dateToString", make_document(kvp("format", "%Y-%m-%d %H:%M:%S"), kvp("date", "$updated_at"), kvp("timezone","+08:00"))))),
            kvp("_id", false),
            kvp("id", true),
            kvp("name", true),
            kvp("group", true),
            kvp("filter_key", true)
        ))),
        make_document(kvp("$sort", make_document(
            kvp("updated_at", -1)
        )))
    ));

    co_return std::make_unique<json_response>(result, EST_OK);
}

boost::cobalt::task<std::unique_ptr<http_response>> article_category::get_article_category_full(http_request_wrapper& request) {

    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article_category"];
    auto filter = make_document(kvp("$or", make_array(
        make_document(kvp("deleted_at", bsoncxx::types::b_null{})),
        make_document(kvp("deleted_at", make_document(kvp("$gt", bsoncxx::types::b_date(std::chrono::system_clock::now())))))
    )));

    auto pipelines = make_array(
        make_document(kvp("$match", filter)),
        make_document(kvp("$addFields", make_document(kvp("id", make_document(kvp("$toString", "$_id")))))),
        make_document(kvp("$project", make_document(
           kvp("updated_at", make_document(kvp("$dateToString", make_document(kvp("format", "%Y-%m-%d %H:%M:%S"), kvp("date", "$updated_at"))))),
           kvp("_id", false),
           kvp("name", true),
           kvp("group", true),
           kvp("filter_key", true)
       )))
    );
    mongocxx::pipeline pipeline;
    pipeline.append_stages(pipelines.view());

    auto result = collection.aggregate(pipeline);
    array arr;
    for(auto &doc: result) {
        document document;
        arr.append(doc);
    }

    co_return std::make_unique<json_response>(make_document(kvp("data", arr)), EST_OK);
}

boost::cobalt::task<std::unique_ptr<http_response>> article_category::create_article_category(http_request_wrapper& request) {
    co_await request.validate({
        {"name", {required()}},
            {"group", {required()}}
    });

    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");

    auto collection = (*conn)["hl_blog_database"]["c_article_category"];
    const auto result = collection.insert_one(make_document(
        kvp("name", request.params()["name"].as_string()),
        kvp("group", request.params()["group"].as_string()),
        kvp("filter_key", request.params()["filter_key"].as_string()),
        kvp("created_at", bsoncxx::types::b_date(std::chrono::system_clock::now())),
        kvp("updated_at", bsoncxx::types::b_date(std::chrono::system_clock::now()))
    ));
    if (!result.has_value())
        throw http_exception("error.database.mongo.execution_error", EST_INTERNAL_SERVER_ERROR);

    co_return std::make_unique<json_response>(boost::json::object{
        {
            "data", {
                {"id", result.value().inserted_id().get_oid().value.to_string()},
                {"name", request.params()["name"].as_string()},
                {"group", request.params()["group"].as_string()}
            }
        }
    });
}

boost::cobalt::task<std::unique_ptr<http_response>> article_category::update_article_category(http_request_wrapper& request) {
    co_await request.validate({{"category_id", {required()}}});
    const auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article_category"];

    bsoncxx::oid oid(request.params()["category_id"].as_string());
    document document{};

    if (request.params().contains("name")) {
        document.append(kvp("name", request.params()["name"].as_string()));
    }
    if (request.params().contains("group")) {
        document.append(kvp("group", request.params()["group"].as_string()));
    }

    if (request.params().contains("filter_key")) {
        document.append(kvp("filter_key", request.params()["filter_key"].as_string()));
    }


    document.append(kvp("updated_at", bsoncxx::types::b_date(std::chrono::system_clock::now())));
    collection.update_one(make_document(kvp("_id", oid)), make_document(kvp("$set", document)));

    co_return std::make_unique<json_response>(boost::json::object{
        {
            "data", {
                {"id", oid.to_string()}
            }
        }
    });
}

boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> article_category::delete_article_category(obelisk::http::http_request_wrapper& request) {
    co_await request.validate({{"category_id", {required()}}});
    auto conn = connection_manager::get_connection<mongo::mongo_connection>("mongo");
    auto collection = (*conn)["hl_blog_database"]["c_article_category"];
    collection.update_one(make_document(kvp("_id", bsoncxx::oid(request.params()["category_id"].as_string()))),
    make_document(kvp("$set", make_document(kvp("deleted_at", bsoncxx::types::b_date(std::chrono::system_clock::now()))))));
    co_return std::make_unique<json_response>(boost::json::object{
        {
            "data", {
                {"id", request.params()["category_id"].as_string()}
            }
        }
    });
}
