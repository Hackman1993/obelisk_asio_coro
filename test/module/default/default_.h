//
// Created by hackman on 4/27/25.
//

#ifndef DEFAULT__H
#define DEFAULT__H
#include <obelisk/http/module/base_module.h>

namespace module
{
    class default_module final : public obelisk::http::module::base_module
    {
    public:
        void route(obelisk::http::http_server& server) override;
        boost::asio::awaitable<void> migrate() override;
    };
} // module

#endif //DEFAULT__H
