//
// Created by hackman on 4/27/25.
//

#ifndef MODULE_CONTENT_H
#define MODULE_CONTENT_H
#include <obelisk/http/module/base_module.h>

namespace module
{
    class content_module final : public obelisk::http::module::base_module
    {
    public:
        void route(obelisk::http::http_server& server) override;
        boost::asio::awaitable<void> migrate() override;
    };
} // module

#endif //MODULE_CONTENT_H
