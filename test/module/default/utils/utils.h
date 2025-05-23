//
// Created by hackman on 4/30/25.
//

#ifndef DEFAULT_UTILS_H
#define DEFAULT_UTILS_H
#include <boost/asio/awaitable.hpp>
#include <nlohmann/json.hpp>
#include <obelisk/http/response/json_response.h>
#include <boost/mysql/pfr.hpp>
#include <clients/aliyun_sms_client.h>
#include <obelisk/http/exception/http_exception.h>
#include <obelisk/http/framework/config.h>
#include <obelisk/database/builder/detail/col.h>
#include <obelisk/database/db.h>
namespace module::default_
{
    template <typename T>
    struct is_optional: std::false_type{};

    template <typename T>
    struct is_optional<std::optional<T>> : std::true_type{};

    class utils
    {
    public:
        template<typename T, typename = std::enable_if_t<
            boost::pfr::is_implicitly_reflectable_v<T, struct t>
        >>
        static nlohmann::json::object_t to_json(T& t)
        {
            nlohmann::json::object_t result;
            boost::pfr::for_each_field(t, [&](const auto& field, auto index){
                result.emplace(boost::pfr::get_name<index, T>(), to_json_val(field));
            });
            return result;
        }

        template<typename T>
        static nlohmann::json to_json_val(const T& value)
        {
            if constexpr (is_optional<T>::value){
                if (value.has_value())
                    return value.value();
                return nullptr;
            }
            else
                return value;
        }

        template<typename T, typename = std::enable_if_t<
            boost::pfr::is_implicitly_reflectable_v<T, struct t>
        >>
        static nlohmann::json to_json(boost::mysql::static_results<boost::mysql::pfr_by_name<T>>& t)
        {
            nlohmann::json::array_t result;
            for(auto &val: t.rows())
            {
                nlohmann::json::object_t obj;
                boost::pfr::for_each_field(val, [&](const auto& field, auto index){
                    obj.emplace(boost::pfr::get_name<index, T>(), to_json_val(field));
                });
                result.emplace_back(obj);
            }

            return result;
        }

        static std::unique_ptr<obelisk::http::json_response> json_response(const nlohmann::json& json, const obelisk::http::EResponseCode code = obelisk::http::EST_OK)
        {
            return std::make_unique<obelisk::http::json_response>(nlohmann::json{
                {"data", json},
                {"code", code}
            }, code);
        }

        static boost::asio::awaitable<void> send_sms_by_channel(const std::string& channel, const std::string&phone, const nlohmann::json& data)
        {
            using namespace obelisk::http;
            const std::string& channel_prefix = std::format("sms.channel.{}", channel);
            const std::string& channel_type = config::get<std::string>(channel_prefix+".provider", "aliyun");
            const auto& template_code = config::get<std::string>(channel_prefix+".template_code", "");
            const auto& sign_name = config::get<std::string>(channel_prefix+".sign_name", "");
            if (channel_type == "aliyun")
            {
                const auto &ak_id = config::get<std::string>(channel_prefix+".aliyun_ak_id", "");
                const auto &ak_secret = config::get<std::string>(channel_prefix+".aliyun_ak_secret", "");
                aliyun_sms_client sms_client(ak_id, ak_secret);
                co_await sms_client.send_sms(phone, sign_name, template_code, data.dump());
            }else
            {
                throw http_exception("server.error.sms_provider_not_supported", EST_INTERNAL_SERVER_ERROR);
            }
        }

        static boost::asio::awaitable<bool> validate_verify_code(const std::string& phone, const std::string& verify_code, const std::string& type)
        {
            using namespace obelisk::database::builder::detail;
            const auto count = co_await obelisk::database::db::select({col("id")}).from({"sys_verify_codes"}).where({
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

        static boost::asio::awaitable<bool> can_cascade(const std::string& code,std::uint64_t sys_admin_id)
        {
            using namespace obelisk::database::builder::detail;
            auto query = obelisk::database::db::select({col("sp.id")}).from({{"sys_mid_role_permission", "smrp"}})
            .left_join({"sys_roles", "sr"},{{col("smrp.fn_role_id"), col("sr.id")},{col("sr.deleted_at"), nullptr}})
            .left_join({"sys_permissions", "sp"}, {{col("sp.id"), col("smrp.fn_permission_id")}})
            .left_join({"sys_mid_admin_role", "smar"}, {{col("smar.fn_role_id"), col("sr.id")}})
            .left_join({"sys_admins", "sa"}, {{col("sa.id"), col("smar.fn_admin_id")}, {col("sa.deleted_at"), nullptr}})
            .left_join({"sys_organizations", "so"}, {{col("so.id"), col("sa.fn_organization_id")}, {col("so.deleted_at"), nullptr}})
            .where({
                {col("sa.id"), sys_admin_id},
                {col("sp.code"), code},
                {col("smrp.cascade"), 1},
            }).order_by({{"smrp.cascade"}, "DESC"}).limit(1);

            const auto result = co_await query.get();
            co_return !result.rows().empty();
        }

        static boost::asio::awaitable<bool> can(const std::string& code,std::uint64_t sys_admin_id, std::uint64_t target_org_id)
        {
            using namespace obelisk::database::builder::detail;


            auto query = obelisk::database::db::select({col("sa.fn_organization_id"),col("smrp.cascade")}).from({{"sys_mid_role_permission", "smrp"}})
            .join({"sys_roles", "sr"},{{col("smrp.fn_role_id"), col("sr.id")},{col("sr.deleted_at"), nullptr}})
            .join({"sys_permissions", "sp"}, {{col("sp.id"), col("smrp.fn_permission_id")}})
            .join({"sys_mid_admin_role", "smar"}, {{col("smar.fn_role_id"), col("sr.id")}})
            .join({"sys_admins", "sa"}, {{col("sa.id"), col("smar.fn_admin_id")}, {col("sa.deleted_at"), nullptr}})
            .join({"sys_organization_hierarchy", "soh"}, {
                {col("soh.fn_ancestor_id"), col("sa.fn_organization_id")},
                {col("soh.fn_ancestor_id"), col("sr.fn_organization_id")}
            })
            .where({
                {col("sa.id"), sys_admin_id},
                {col("sp.code"), code},
                {col("soh.fn_descendant_id"), target_org_id},
                {col("soh.fn_ancestor_id"), col("sa.fn_organization_id")},
            }).order_by({{"smrp.cascade"}, "DESC"}).limit(1);
            const auto result = co_await query.get();
            if (result.rows().empty())
                co_return false;

            if (target_org_id != result.rows()[0][0].as_uint64())
                co_return result.rows()[0][1].as_int64() == 1;
            co_return true;
        }

    private:
        static boost::asio::awaitable<void> _insert_permission(const std::string& code, bool cascade)
        {
            using namespace obelisk::database::builder::detail;
            std::uint64_t permission_id;
            auto result = co_await obelisk::database::db::select({col("id"), col("code")}).from({"sys_permissions"}).where({{"code", code}}).get();

            if (result.rows().empty())
            {
                permission_id = (co_await obelisk::database::db::insert("sys_permissions").values({
                    {"code", code},
                    {"visible", true},
                    {"cascading", cascade}
                }).get()).last_insert_id();
            }
            else
                permission_id = result.rows()[0][0].as_uint64();

            result = co_await obelisk::database::db::select({col("fn_role_id"), col("fn_permission_id")}).from({"sys_mid_role_permission"}).where({
                {"fn_role_id", 1},
                {"fn_permission_id", permission_id}
            }).get();
            if (result.rows().empty())
            {
                co_await obelisk::database::db::insert("sys_mid_role_permission").values({
                    {"fn_role_id", 1},
                    {"fn_permission_id", permission_id},
                    {"cascade", cascade},
                    {"grant", true}
                }).get();
            }
            else
            {
                co_await obelisk::database::db::update({"sys_mid_role_permission"}).set({
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
