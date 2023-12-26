//
// Created by Hackman.Lo on 2023/12/22.
//

#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H
#include <boost/json.hpp>
#include "http/core/http_request.h"
#include "http/response/json_response.h"
#include "http/validator/validator.h"
#include <boost/asio/awaitable.hpp>
#include <boost/mysql.hpp>


boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>>
login(obelisk::http::http_request_wrapper&request) {

    using namespace obelisk::http::validator;
    request.validate({
        {"username", {required()}},
        {"password", {required()}}
    });



    co_return std::move(std::make_unique<obelisk::http::json_response>(boost::json::object{
                                                                           {"data", nullptr}
                                                                       }, obelisk::http::EResponseCode::EST_OK));
}


#endif //AUTH_CONTROLLER_H
