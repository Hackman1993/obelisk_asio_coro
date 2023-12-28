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
#include <database/database.h>
#include <database/mysql/mysql_connection.h>

#include "core/coroutine/async_mutex2.h"

boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>>
login(obelisk::http::http_request_wrapper&request) {

    using namespace obelisk::http::validator;
    request.validate({
        {"username", {required()}},
        {"password", {required()}}
    });
    auto conn = obelisk::database::connection_manager::get_connection<obelisk::database::mysql::mysql_connection>("mysql");
    obelisk::core::coroutine::async_mutex2 mutex;
    std::mutex mutex_;
    obelisk::core::coroutine::async_lock([](auto t) {
        std::cout << "OK" << std::endl;
    }, mutex_);
    //  boost::asio::deadline_timer timer(request.io_context());
    // timer.async_wait( [](const boost::system::error_code& e) {
    //
    // })
    // co_await timer.async_wait(boost::asio::use_awaitable);



    co_return std::move(std::make_unique<obelisk::http::json_response>(boost::json::object{
                                                                           {"data", nullptr}
                                                                       }, obelisk::http::EResponseCode::EST_OK));
}


#endif //AUTH_CONTROLLER_H
