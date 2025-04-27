/***********************************************************************************************************************
 * @author Hackman Lo
 * @file route_item.h
 * @description 
 * @created_at 2023-10-09
***********************************************************************************************************************/

#ifndef OBELISK_ROUTE_ITEM_H
#define OBELISK_ROUTE_ITEM_H

#include <regex>
#include <string>
#include <memory>
#include <functional>
#include <boost/asio/awaitable.hpp>
#include <obelisk/http/http_server.h>
#include <obelisk/http/middleware/middleware.h>

#include "route_param.h"
#include "../core/http_request.h"
#include "../core/http_response.h"

namespace obelisk::http {

    class route_item {
    public:
        route_item(const std::string &path, const std::function<obelisk::task<std::unique_ptr<http_response>> (http_request_wrapper &)> &handler);

        bool match(const std::string &path, std::unordered_map<std::string, std::string> &route_params);

        bool method_allowed(const std::string &method);
        bool method_allowed(std::string_view method);
        route_item &method(const std::vector<std::string>& methods);
        std::string allowed_methods();
        obelisk::task<std::unique_ptr<http_response>> handle(http_request_wrapper &request);

        template<typename T>
        typename std::enable_if_t<std::is_base_of_v<middleware::http_middleware_base, T>, void>
        middleware(T middleware)
        {
            middlewares_.emplace_back(std::make_unique<T>(middleware));
        }

    protected:
        std::regex address_;
        std::vector<route_param> pattern_;
        std::vector<std::unique_ptr<middleware::http_middleware_base>> middlewares_;
        std::unordered_map<std::string, bool> available_method_ = {{"OPTIONS", true}, {"HEAD", true}};
        std::function<obelisk::task<std::unique_ptr<http_response>>(http_request_wrapper &)> handler_;
    };

} // obelisk

#endif //OBELISK_ROUTE_ITEM_H
