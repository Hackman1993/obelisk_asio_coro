//
// Created by Hackman.Lo on 2024/3/1.
//

#ifndef CONTROLLER_BASE_H
#define CONTROLLER_BASE_H
#include <boost/cobalt.hpp>
#include <mongocxx/collection.hpp>

#include "http/core/http_request.h"


boost::cobalt::task<bsoncxx::document::value> paginate(mongocxx::collection& collection, obelisk::http::http_request_wrapper& request, bsoncxx::builder::basic::document& filter, mongocxx::options::find& option);



#endif //CONTROLLER_BASE_H
