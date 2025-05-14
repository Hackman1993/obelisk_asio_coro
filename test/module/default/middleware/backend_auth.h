//
// Created by Hackman.Lo on 2024/3/4.
//

#ifndef MIDDLEWARE_BACKEND_AUTH_H
#define MIDDLEWARE_BACKEND_AUTH_H
#include <obelisk/http/exception/http_exception.h>
#include "module/default/model/backend_user_info.h"
#include <boost/pfr.hpp>
#include <boost/mysql/pfr.hpp>
namespace module::default_::middleware{
    class backend_auth final : public obelisk::http::middleware::base_middleware {
    public:
        backend_auth(std::string target_key): target_key_(std::move(target_key))
        {
        }
        boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> pre_handle(obelisk::http::http_request_wrapper&request) override {
            if (!request.headers().contains("authorization"))
                throw obelisk::http::http_exception("server.error.access_forbidden", obelisk::http::EST_FORBIDDEN);


            const auto authorization_header = request.headers()["Authorization"];
            if (authorization_header.find("Bearer ") == std::string::npos)
                throw obelisk::http::http_exception("server.error.access_forbidden", obelisk::http::EST_FORBIDDEN);
            auto token = authorization_header.substr(authorization_header.find("Bearer ") + 7);

            const boost::mysql::results results = co_await obelisk::database::db::select({
				{"so.id", "organization_id"},
				{"sa.id", "admin_id"},
                {"sat.id", "token_id"},
			}).from({{"sys_access_tokens", "sat"}})
            // Join sys_admins table
            .inner_join({"sys_admins", "sa"}, {
				{{col{"sa.id"},col{"sat.fn_target_id"}}, {col{"sa.deleted_at"}, nullptr}}
			})
            // Join sys_organization table
            .inner_join({"sys_organizations", "so"}, {
                {{col{"sa.fn_organization_id"}, col{"so.id"}}, {col{"so.deleted_at"}, nullptr}}
            })
            .where({
                {col{"sat.token"}, token},
                {col{"sat.target_key"}, target_key_},
                {col{"expires_at"}, ">", std::chrono::system_clock::now()},
            }).get();

            if (results.rows().empty())
                throw obelisk::http::http_exception("server.error.access_forbidden", obelisk::http::EST_FORBIDDEN);

            co_await obelisk::database::db::update({"sys_access_tokens"}).set({
                {"expires_at", std::chrono::system_clock::now() + std::chrono::minutes(30)},
                {"last_used_at", std::chrono::system_clock::now()}
            }).where({
                {col{"id"}, results.rows()[0][2].as_uint64()}
            }).get();
            request.additional_data().emplace(std::format("_{}_organization_id", target_key_), results.rows()[0][0].as_uint64());
            request.additional_data().emplace(std::format("_{}_id", target_key_), results.rows()[0][1].as_uint64());
            co_return nullptr;
        }

    private:
        std::string target_key_;
        std::optional<std::vector<std::string>> permissions_;
    };

}


#endif //MIDDLEWARE_BACKEND_AUTH_H
