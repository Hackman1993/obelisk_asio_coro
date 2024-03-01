//
// Created by Hackman.Lo on 2024/3/1.
//

#ifndef CONTROLLER_BASE_H
#define CONTROLLER_BASE_H
#include <boost/cobalt.hpp>
#include <mongocxx/collection.hpp>

#include "http/core/http_request.h"


bsoncxx::builder::basic::document paginate(mongocxx::collection& collection, obelisk::http::http_request_wrapper& request, bsoncxx::builder::basic::document& filter);



#endif //CONTROLLER_BASE_H
