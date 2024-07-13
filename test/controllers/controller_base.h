//
// Created by Hackman.Lo on 2024/3/1.
//

#ifndef CONTROLLER_BASE_H
#define CONTROLLER_BASE_H
#include <mongocxx/collection.hpp>

#include "http/core/http_request.h"

class controller_base {
protected:
    static boost::asio::awaitable<bsoncxx::document::value> paginate(mongocxx::collection& collection, obelisk::http::http_request_wrapper& request, bsoncxx::document::value filter, mongocxx::options::find& option);
    static boost::asio::awaitable<bsoncxx::document::value> paginate(mongocxx::collection&collection,
                                                                obelisk::http::http_request_wrapper&request,
                                                                bsoncxx::document::value filter, bsoncxx::array::value lookup);
};





#endif //CONTROLLER_BASE_H
