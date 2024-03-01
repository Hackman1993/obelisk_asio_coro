//
// Created by Hackman.Lo on 2023/12/26.
//

#ifndef MYSQL_CONNECTION_H
#define MYSQL_CONNECTION_H
#include "../db_connection_base.h"
#include <boost/mysql.hpp>

namespace obelisk::database::mysql {
    class mysql_connection : public db_connection_base, public boost::mysql::tcp_connection{
    public:
        mysql_connection(boost::asio::io_context&ctx, const std::string&username, const std::string&password, const std::string&database, const std::string&host = "localhost", const std::string&port = "3306");

        bool reset() override { return true; };

        bool execute() {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(10s);
            return true;
        }

        void refresh() override;
    };
} // obelisk::database::mysql

#endif //MYSQL_CONNECTION_H
