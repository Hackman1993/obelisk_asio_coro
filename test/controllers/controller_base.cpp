//
// Created by Hackman.Lo on 2024/3/1.
//
#include "controller_base.h"

#include "http/validator/integer_validator.h"

#include <bsoncxx/json.hpp>
boost::cobalt::task<bsoncxx::document::value> controller_base::paginate(mongocxx::collection&collection,
                                                                obelisk::http::http_request_wrapper&request,
                                                                bsoncxx::document::value filter, mongocxx::options::find& option) {
    using namespace obelisk::http::validator;
    co_await request.validate({
        {"page", {integer()}},
        {"limit", {integer()}},
    });
    const std::int64_t total = collection.count_documents(filter.view());
    std::int64_t limit = 10;
    std::int64_t page = 1;
    if (request.params().contains("limit")) {
        limit = request.params()["limit"].as_int64();
        limit = std::clamp<std::int64_t>(limit, 1, 100);
    }
    const auto maxium_page = total % limit > 0 ? total / limit + 1 : total / limit;
    if (request.params().contains("page")) {
        const auto request_page = request.params()["page"].as_int64();
        page = std::clamp<std::int64_t>(request_page, 1, maxium_page);
    }

    using namespace bsoncxx::builder::basic;
    option.limit(limit);
    option.skip((page - 1) * limit);
    auto cursor = collection.find(filter.view(), option);

    array data_array{};
    for (auto&item: cursor) {
        document doc_val;
        for (auto&value: item) {
            if (value.key() == "_id")
                doc_val.append(kvp("id", value.get_oid().value.to_string()));
            else
                doc_val.append(kvp(value.key(), value.get_value()));
        }
        data_array.append(doc_val.view());
    }

    auto document = make_document(
        kvp("total", total),
        kvp("page", page),
        kvp("limit", limit),
        kvp("data", data_array)
    );

    co_return document;
}

boost::cobalt::task<bsoncxx::document::value> controller_base::paginate(mongocxx::collection&collection,
                                                                obelisk::http::http_request_wrapper&request,
                                                                bsoncxx::document::value filter, bsoncxx::array::value lookup) {
    using namespace obelisk::http::validator;
    co_await request.validate({
        {"page", {integer()}},
        {"limit", {integer()}},
    });

    const std::int64_t total = collection.count_documents(filter.view());
    std::int64_t limit = 10;
    std::int64_t page = 1;
    if (request.params().contains("limit")) {
        limit = request.params()["limit"].as_int64();
        limit = std::clamp<std::int64_t>(limit, 1, 100);
    }
    auto maxium_page = total % limit > 0 ? total / limit + 1 : total / limit;
    maxium_page = maxium_page<=0? 1: maxium_page;
    if (request.params().contains("page")) {
        const auto request_page = request.params()["page"].as_int64();
        page = std::clamp<std::int64_t>(request_page, 1, maxium_page);
    }

    using namespace bsoncxx::builder::basic;
    mongocxx::pipeline pipeline;
    pipeline.append_stage(make_document(kvp("$addFields", make_document(kvp("id", make_document(kvp("$toString", "$_id")))))));
    pipeline.append_stage(make_document(kvp("$match", filter.view())));
    pipeline.append_stages(lookup.view());
    pipeline.append_stage(make_document(kvp("$skip", (page - 1) * limit)));
    pipeline.append_stage(make_document(kvp("$limit", limit)));

    auto cursor = collection.aggregate(pipeline);
    array data_array{};
    for (auto&item: cursor) {
        document doc_val;
        for (auto&value: item) {
            doc_val.append(kvp(value.key(), value.get_value()));
        }
        data_array.append(doc_val.view());
    }

    auto document = make_document(
        kvp("total", total),
        kvp("page", page),
        kvp("limit", limit),
        kvp("data", data_array)
    );

    co_return document;
}
