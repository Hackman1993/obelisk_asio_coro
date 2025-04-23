//
// Created by Hackman.Lo on 2024/7/11.
//

#ifndef REDIS_CONNECTION_H
#define REDIS_CONNECTION_H

#include <boost/redis.hpp>
#include <boost/asio/detached.hpp>
#include <obelisk/database/core/db_connection_base.h>

class redis_connection : public obelisk::database::db_connection_base, public boost::redis::connection{
public:
    redis_connection(boost::asio::io_context& ioctx, const std::string& host, std::uint16_t port, int db_index = 0, const std::string& password = "");
    void refresh() override;

    bool reset() override;

    ~redis_connection() override;
};



#endif //REDIS_CONNECTION_H
