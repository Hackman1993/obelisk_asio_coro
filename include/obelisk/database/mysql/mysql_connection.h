//
// Created by Hackman.Lo on 2024/7/11.
//

#ifndef MYSQL_CONNECTION_H
#define MYSQL_CONNECTION_H
#include <iostream>
#include <boost/mysql.hpp>
#include <obelisk/database/core/db_connection_base.h>

class mysql_connection : public obelisk::database::db_connection_base, public boost::mysql::any_connection{
public:
    mysql_connection(boost::asio::io_context& ioctx, const std::string& server, const std::uint16_t port, const std::string& username,const std::string& password,const std::string& database):
        db_connection_base(ioctx), boost::mysql::any_connection(ioctx) {
        boost::asio::ip::tcp::resolver resolver(ioctx);
        auto endpoints = resolver.resolve(server, std::to_string(port));

        boost::system::error_code ec;
        boost::mysql::diagnostics diagnostics;
        boost::mysql::connect_params connect_params;
        connect_params.server_address.emplace_host_and_port(server, port);
        connect_params.database = database;
        connect_params.username = username;
        connect_params.password = password;
        connect(connect_params, ec, diagnostics);
        boost::mysql::throw_on_error(ec, diagnostics);
        set_character_set(boost::mysql::character_set("utf8mb4"), ec, diagnostics);
        boost::mysql::throw_on_error(ec, diagnostics);
    };
    void refresh() override;

    bool reset() override;

    ~mysql_connection() override;
};



#endif //MYSQL_CONNECTION_H
