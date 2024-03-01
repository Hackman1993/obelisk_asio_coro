//
// Created by Hackman.Lo on 2024/3/1.
//
#include "controller_base.h"
bsoncxx::builder::basic::document paginate(mongocxx::collection& collection, obelisk::http::http_request_wrapper& request, bsoncxx::builder::basic::document& filter){
    const std::int64_t total = collection.count_documents(filter.view());
    std::int64_t limit = 10;
    std::int32_t page = 1;
    const auto maxium_page = total%limit >0? total/limit+1: total/limit;

    if(request.params().contains("limit")){
        limit = std::strtol(request.params()["limit"][0].c_str(), nullptr, 10);
        limit = std::clamp<std::int32_t>(limit, 1, 100);
    }
    if(request.params().contains("page")){
        const auto request_page = std::strtol(request.params()["page"][0].c_str(), nullptr, 10);
        page = std::clamp<std::int32_t>(request_page, 1, maxium_page);
    }

    using namespace bsoncxx::builder::basic;
    mongocxx::options::find option{};
    option.limit(limit);
    option.skip((page-1) * limit);
    auto cursor = collection.find(filter.view(), option);

    document doc;
    array data_array{};
    doc.append(kvp("total", total));
    doc.append(kvp("page", page));
    doc.append(kvp("limit", limit));
    for(auto &item: cursor) {
        document doc_val;
        for(auto & value: item) {
            if(value.key() == "_id")
                doc_val.append(kvp("id", value.get_oid().value.to_string()));
            else
                doc_val.append(kvp(value.key(),value.get_value()));
        }
        data_array.append(doc_val.view());
    }
    doc.append(kvp("data", data_array));

    return doc;
}