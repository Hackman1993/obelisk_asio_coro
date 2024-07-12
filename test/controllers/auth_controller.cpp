#include "auth_controller.h"

#include <sahara/hash/bcrypt.h>
#include <boost/algorithm/string/replace.hpp>

#include "obelisk.h"
#include "../common/user_data.h"
#include "http/validator/validator.h"
#include "database/connection_pool.h"
#include "http/exception/http_exception.h"
#include "database/redis/redis_connection.h"
#include "database/mongo/mongo_connection.h"

boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> login(obelisk::http::http_request_wrapper&request) {
    co_await request.validate({
        {"login", {required()}},
        {"password", {required()}}
    });

    std::timed_mutex mutex;

    const auto redis_conn = obelisk::database::connection_manager::get_connection<
        obelisk::database::redis::redis_connection>("redis");
    if (!redis_conn)
        throw obelisk::http::http_exception("database.redis.connection_failed",
                                            obelisk::http::EResponseCode::EST_INTERNAL_SERVER_ERROR);
    const auto mongo_conn = obelisk::database::connection_manager::get_connection<
        obelisk::database::mongo::mongo_connection>("mongo");
    if (!mongo_conn)
        throw obelisk::http::http_exception("database.mongo.connection_failed",
                                            obelisk::http::EResponseCode::EST_INTERNAL_SERVER_ERROR);

    using namespace bsoncxx::builder::basic;
    auto collection = (*mongo_conn)["hl_blog_database"]["c_sys_admin"];
    const auto document_optional = collection.find_one(make_document(kvp("username", "admin")));
    if (!document_optional)
        throw obelisk::http::http_exception("auth.invalid_credentials", obelisk::http::EResponseCode::EST_UNAUTHORIZED);
    const auto&document = document_optional.value();
    const auto password = document.find("password")->get_string();
    const auto &[oid] = document.find("_id")->get_oid();

    if (!sahara::hash::bcrypt::validatePassword(request.params()["password"].as_string(), password.value))
        throw obelisk::http::http_exception("auth.invalid_credentials", obelisk::http::EResponseCode::EST_UNAUTHORIZED);

    std::string access_token = sahara::utils::uuid::generate();
    boost::algorithm::replace_all(access_token, "-", "");
    if(!redis_conn->set(access_token, oid.to_string(), std::chrono::hours(2))) {
        throw obelisk::http::http_exception("database.redis.execution_error",
                                            obelisk::http::EResponseCode::EST_INTERNAL_SERVER_ERROR);
    }

    const auto expire_at = std::chrono::system_clock::now() + std::chrono::hours(2);
    auto expire_at_string = sahara::string_ext::time_format(expire_at, "%F %T");
    co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
                                                                 {"expire", expire_at_string},
                                                                 {"token", access_token}
                                                             }, obelisk::http::EResponseCode::EST_OK);
}

boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> logout(obelisk::http::http_request_wrapper&request) {
    auto udt = std::any_cast<user_data>(request.additional_data()["__user_info"]);
    const auto conn = obelisk::database::connection_manager::get_connection<obelisk::database::redis::redis_connection>("redis");
    if(!conn)
        throw obelisk::http::http_exception("error.database.redis.connection_error", obelisk::http::EST_INTERNAL_SERVER_ERROR);
    auto result = conn->del(udt.access_token);
    if(!result)
        throw obelisk::http::http_exception("error.database.redis.execution_error", obelisk::http::EST_INTERNAL_SERVER_ERROR);
    co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        {"data", nullptr}
    }, obelisk::http::EST_OK);

}

boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> check_auth(obelisk::http::http_request_wrapper&request) {
    co_return std::make_unique<obelisk::http::json_response>(boost::json::object{
        {"data", nullptr}
    }, obelisk::http::EST_OK);
}