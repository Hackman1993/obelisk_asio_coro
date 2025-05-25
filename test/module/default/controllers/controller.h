//
// Created by hackman on 5/19/25.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <obelisk/database/db.h>
#include <obelisk/http/core/http_response.h>
#include <obelisk/http/core/http_request.h>
#include <obelisk/http/response/json_response.h>
namespace module::default_::controllers
{
    using namespace boost::asio;
    using namespace obelisk::database::builder::detail;
    using namespace obelisk::database;
    using namespace obelisk::http::validator;

    template <typename T>
    struct is_optional: std::false_type{};
    template <typename T>
    struct is_optional<std::optional<T>> : std::true_type{};

    struct pagination_uniformed
    {
        std::uint64_t total;
        std::uint64_t page;
        std::uint64_t limit;
    };

    class controller {

    protected:
        template<typename As>
        static void try_emplace(const std::string& key, std::vector<std::pair<col, sql_value>>& values, nlohmann::json params)
        {
            if (params.contains(key))
                values.emplace_back(key, params[key].get<As>());
        }

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

        static awaitable<bool> can_cascade(const std::string& code,std::uint64_t sys_admin_id);

        static std::unique_ptr<obelisk::http::json_response> json_response(const nlohmann::json& json, obelisk::http::EResponseCode code = obelisk::http::EST_OK);

        static awaitable<pagination_uniformed> pagination_uniform(builder::detail::query statement, obelisk::http::http_request_wrapper& request);
        template<typename T>
        static awaitable<std::unique_ptr<obelisk::http::json_response>> pagination(builder::detail::query statement, pagination_uniformed uniform)
        {
            auto result = co_await statement.limit(uniform.limit, (uniform.page-1)*uniform.limit).get<T>();
            co_return json_response({
                {"list", to_json(result)},
                {"total", uniform.total},
                {"page", uniform.page},
                {"limit", uniform.limit}
            });
        }

        static awaitable<std::uint64_t> can(const std::string& code, obelisk::http::http_request_wrapper& request, const std::string& model, std::uint64_t target_id);
        static awaitable<bool> can(const std::string& code, obelisk::http::http_request_wrapper& request, std::uint64_t target_org_id);
    };

}

#endif //CONTROLLER_H
