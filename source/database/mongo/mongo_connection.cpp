//
// Created by Hackman.Lo on 2024/2/21.
//

#include "database/mongo/mongo_connection.h"
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

namespace obelisk::database::mongo {
    mongocxx::instance mongo_connection::instance_ = mongocxx::instance{};

    mongo_connection::mongo_connection(boost::asio::io_context& ioctx, const std::string &uri): db_connection_base(ioctx), mongocxx::client(mongocxx::uri(uri)) {

    }

    void mongo_connection::refresh() {
    }

    bool mongo_connection::reset() {
        return true;
    }
} // obelisk::database::mongo
