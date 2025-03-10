//
// Created by Hackman.Lo on 2024/7/11.
//

#include "redis_connection.h"
#include "boost/redis/src.hpp"
redis_connection::redis_connection(boost::asio::io_context &ioctx, const std::string &host, std::uint16_t port, int db_index, const std::string &password): db_connection_base(ioctx), boost::redis::connection(ioctx){
    boost::redis::config config;
    config.addr.host = host;
    config.addr.port = std::to_string(port);
    config.database_index = db_index;
    config.password = password;
    async_run(config, {}, boost::asio::consign(boost::asio::detached, this));

}

void redis_connection::refresh() {
}

bool redis_connection::reset() {
    return true;
}

redis_connection::~redis_connection() = default;
