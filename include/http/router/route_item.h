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

#include "route_param.h"
#include "../core/http_request.h"
#include "../core/http_response.h"

namespace obelisk::http {

    class route_item {
    public:
        route_item(const std::string &path, const std::function<boost::asio::awaitable<std::unique_ptr<http_response>> (http_request_wrapper &)> &handler);

        bool match(const std::string &path, std::unordered_map<std::string, std::string> &route_params);

        bool method_allowed(const std::string &method);
        bool method_allowed(std::string_view method);
        route_item &method(const std::vector<std::string>& methods);
        std::string allowed_methods();
        boost::asio::awaitable<std::unique_ptr<http_response>> handle(http_request_wrapper &request);

    protected:
        std::regex address_;
        std::vector<route_param> pattern_;
        std::unordered_map<std::string, bool> available_method_ = {{"OPTIONS", true}, {"HEAD", true}};
        std::function<boost::asio::awaitable<std::unique_ptr<http_response>>(http_request_wrapper &)> handler_;
    };

} // obelisk

#endif //OBELISK_ROUTE_ITEM_H
