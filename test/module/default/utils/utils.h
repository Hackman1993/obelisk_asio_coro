//
// Created by hackman on 4/30/25.
//

#ifndef DEFAULT_UTILS_H
#define DEFAULT_UTILS_H
#include <boost/asio/awaitable.hpp>
#include <nlohmann/json.hpp>
#include <obelisk/http/response/json_response.h>

namespace module::default_
{
    class utils
    {
    public:
        static std::unique_ptr<obelisk::http::json_response> json_response(const nlohmann::json& json, const obelisk::http::EResponseCode code = obelisk::http::EST_OK)
        {
            return std::make_unique<obelisk::http::json_response>(nlohmann::json{
                {"data", json},
                {"code", code}
            }, code);
        }

        static boost::asio::awaitable<bool> validate_verify_code(const std::string& phone, const std::string& verify_code, const std::string& type)
        {
            const auto count = co_await obelisk::database::db::select({"id"}).from({"sys_verify_codes"}).where({
                {col{"phone"}, phone},
                {col{"code"}, verify_code},
                {col{"type"}, type},
                {col{"expires_at"}, ">", std::chrono::system_clock::now()},
                {col{"deleted_at"}, nullptr}
            }).count();

            co_return static_cast<bool>(count);
        }

        static boost::asio::awaitable<void> register_permission(std::string code, bool cascade)
        {
            using namespace obelisk::database;
            co_await _insert_permission(code, cascade);
            co_return;
        }

    private:
        static boost::asio::awaitable<void> _insert_permission(const std::string& code, bool cascade)
        {
            using namespace obelisk::database;
            std::uint64_t permission_id;
            auto result = co_await db::select({"id", "code"}).from({"sys_permissions"}).where({{"code", code}}).get();

            if (result.rows().empty())
            {
                permission_id = (co_await db::insert("sys_permissions").values({
                    {"code", code},
                    {"visible", true},
                    {"cascading", cascade}
                }).get()).last_insert_id();
            }
            else
                permission_id = result.rows()[0][0].as_uint64();

            result = co_await db::select({"fn_role_id", "fn_permission_id"}).from({"sys_mid_role_permission"}).where({
                {"fn_role_id", 1},
                {"fn_permission_id", permission_id}
            }).get();
            if (result.rows().empty())
            {
                co_await db::insert("sys_mid_role_permission").values({
                    {"fn_role_id", 1},
                    {"fn_permission_id", permission_id},
                    {"cascade", cascade},
                    {"grant", true}
                }).get();
            }
            else
            {
                co_await db::update({"sys_mid_role_permission"}).set({
                    {"cascade", cascade},
                    {"grant", true}
                }).where({
                    {"fn_role_id", 1},
                    {"fn_permission_id", permission_id}
                }).get();
            }
            co_return;
        }
    };

}
#endif //DEFAULT_UTILS_H
