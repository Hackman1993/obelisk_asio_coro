//
// Created by hackman on 4/27/25.
//

#ifndef BASE_MODULE_H
#define BASE_MODULE_H
#include <boost/asio/awaitable.hpp>
namespace obelisk::http
{
    class http_server;
    namespace module
    {
        class base_module
        {
        public:
            virtual ~base_module() = default;
            virtual boost::asio::awaitable<void> migrate() = 0;
            virtual void route(http_server& server) = 0;
        };
    }
}

#endif //BASE_MODULE_H
