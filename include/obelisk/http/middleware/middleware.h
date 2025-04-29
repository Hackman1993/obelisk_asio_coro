/***********************************************************************************************************************
 * @author Hackman Lo
 * @file middleware.h
 * @description 
 * @created_at 2023-10-10
***********************************************************************************************************************/

#ifndef OBELISK_MIDDLEWARE_H
#define OBELISK_MIDDLEWARE_H
#include <memory>

#include "../core/http_request.h"
#include "../core/http_response.h"
#include <boost/asio/awaitable.hpp>

namespace obelisk::http::middleware {

    class base_middleware{
    public:
        base_middleware();
        virtual ~base_middleware() = default;
        virtual boost::asio::awaitable<std::unique_ptr<http_response>> pre_handle(http_request_wrapper& request) { co_return nullptr; }
        virtual boost::asio::awaitable<void> after_handle(http_request_wrapper& request, http_response& response) { co_return; }
    };

} // obelisk

#endif //OBELISK_MIDDLEWARE_H
