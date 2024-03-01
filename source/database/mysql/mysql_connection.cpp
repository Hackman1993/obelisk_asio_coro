//
// Created by Hackman.Lo on 2023/12/26.
//

#include "database/mysql/mysql_connection.h"



namespace obelisk::database::mysql {
    mysql_connection::mysql_connection(boost::asio::io_context&ctx, const std::string&username, const std::string&password, const std::string&database, const std::string&host, const std::string&port) : db_connection_base(ctx), boost::mysql::tcp_connection(ctx) {
        boost::asio::ip::tcp::resolver resolver(ctx_);
        const auto endpoints = resolver.resolve(host, port);
        const boost::mysql::handshake_params params(username, password, database, boost::mysql::handshake_params::default_collation, boost::mysql::ssl_mode::disable);
        connect(*endpoints.begin(), params);
    }

    void mysql_connection::refresh() {

    }


} // obelisk::database::mysql