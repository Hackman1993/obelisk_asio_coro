//
// Created by Hackman.Lo on 2024/2/29.
//

#include "authorization.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "../common/user_data.h"
#include "../storage/storage.h"
#include "database/database.h"
#include "database/mongo/mongo_connection.h"
#include "database/redis/redis_connection.h"
#include "http/exception/http_exception.h"

boost::cobalt::task<std::unique_ptr<http_response>> authorization::pre_handle(http_request_wrapper& request) {
    if(boost::algorithm::iequals(request.method(), "OPTIONS") | boost::algorithm::iequals(request.method(), "HEAD"))
        co_return nullptr;
    std::unordered_map<std::string, std::string> params;
    const std::regex regex("^/api/backend/[.]*");

    const std::string_view pth = request.target();
    if(const std::string_view path = request.target();!std::regex_search(path.begin(),path.end(),regex))
        co_return nullptr;

    if(!request.headers().contains("Authorization"))
        throw http_exception("error.http.access_denied", EST_FORBIDDEN);
    std::string access_token = request.headers()["Authorization"];
    const auto bearer_location = boost::algorithm::ifind_first(access_token,"Bearer ");
    if (!bearer_location)
        throw http_exception("error.http.access_denied", EST_FORBIDDEN);
    access_token= access_token.substr(bearer_location.size());



    const auto conn = obelisk::database::connection_manager::get_connection<obelisk::database::redis::redis_connection>("redis");
    if(!conn)
        throw http_exception("error.database.redis.connection_error", EST_INTERNAL_SERVER_ERROR);
    const auto val = conn->get(access_token);
    if(!val)
        throw http_exception("error.http.access_denied", EST_FORBIDDEN);
    const auto mongo_conn = obelisk::database::connection_manager::get_connection<obelisk::database::mongo::mongo_connection>("mongo");
    using namespace bsoncxx::builder::basic;
    auto collection = (*mongo_conn)["hl_blog_database"]["c_sys_admin"];
    auto data = collection.find_one(make_document(kvp("_id", bsoncxx::oid(val.value()))));

    user_data udata{val.value(),std::string(data->find("username")->get_string().value), access_token};
    request.additional_data().emplace("__user_info",udata);
    co_return nullptr;
}
