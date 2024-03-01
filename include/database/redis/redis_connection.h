//
// Created by Hackman.Lo on 2024/2/28.
//

#ifndef REDIS_CONNECTION_H
#define REDIS_CONNECTION_H

#include "../db_connection_base.h"
#include <sw/redis++/redis++.h>
namespace obelisk::database::redis {
    class redis_connection: public db_connection_base, public sw::redis::Redis{
    public:
        redis_connection(boost::asio::io_context& context, const sw::redis::ConnectionOptions& option):db_connection_base(context), Redis(option) {

        }
        void refresh() override {}

        bool reset() override{ return true;};

        ~redis_connection() override{};
    };
} // obelisk::database::redis

#endif //REDIS_CONNECTION_H
