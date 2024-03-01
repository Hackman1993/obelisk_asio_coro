//
// Created by Hackman.Lo on 2024/2/21.
//

#ifndef MONGO_CONNECTION_H
#define MONGO_CONNECTION_H
#include "../db_connection_base.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <boost/asio/io_context.hpp>
namespace obelisk::database::mongo {
    class mongo_connection : public db_connection_base, public mongocxx::client{
    public:
        explicit mongo_connection(boost::asio::io_context& ioctx, const std::string &uri);
        void refresh() override;

        bool reset() override;
    protected:
        static mongocxx::instance instance_;
    };
} // obelisk::database::mongo

#endif //MONGO_CONNECTION_H
