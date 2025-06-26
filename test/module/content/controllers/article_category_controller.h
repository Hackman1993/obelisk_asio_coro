//
// Created by hackman on 5/15/25.
//

#ifndef ARTICLE_CATEGORY_CONTROLLER_H
#define ARTICLE_CATEGORY_CONTROLLER_H
#include <obelisk/database/db.h>
#include <obelisk/http/core/http_request.h>
#include <obelisk/http/core/http_response.h>

#include "controllers/controller.h"

namespace module::content::controllers
{
    using namespace boost::asio;
    using namespace obelisk::http;
    class article_category_controller : public ::controllers::controller{
    public:
        static awaitable<std::unique_ptr<http_response>> backend_view(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_create(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_update(http_request_wrapper&request);
        static awaitable<std::unique_ptr<http_response>> backend_delete(http_request_wrapper&request);
    };
}

#endif //ARTICLE_CATEGORY_CONTROLLER_H
