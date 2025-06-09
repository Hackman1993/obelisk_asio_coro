//
// Created by hackman on 6/5/25.
//

#ifndef CARDS_H
#define CARDS_H
#include <boost/asio/awaitable.hpp>

#include "controllers/controller.h"
#include "obelisk/http/core/http_request.h"
#include "obelisk/http/core/http_response.h"

namespace module::crown_plaza::controllers
{

    class cards: public ::controllers::controller {
    public:
        static boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> find_by_card(obelisk::http::http_request_wrapper &request);
    };

}

#endif //CARDS_H
