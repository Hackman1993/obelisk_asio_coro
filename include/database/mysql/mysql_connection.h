//
// Created by Hackman.Lo on 2023/12/26.
//

#ifndef MYSQL_CONNECTION_H
#define MYSQL_CONNECTION_H
#include "../db_connection_base.h"
#include <boost/mysql.hpp>
namespace obelisk::database::mysql {
    class mysql_connection : public db_connection_base{
    public:
        mysql_connection(boost::asio::io_context& ctx): db_connection_base(ctx), connection_(ctx){};
        bool connect(const std::string& username, const std::string& password, const std::string& database, const std::string& host, const std::string& port = "3306") {
            try{
            boost::asio::ip::tcp::resolver resolver(ctx_);
            const auto endpoints = resolver.resolve(host, port);
            const boost::mysql::handshake_params params(username, password, database, boost::mysql::handshake_params::default_collation, boost::mysql::ssl_mode::require);
            connection_.connect(*endpoints.begin(), params);
            } catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
                return false;
            }
            return true;
        }

        bool reset() override{ return true;};

        bool execute() {
            using namespace  std::chrono_literals;
            std::this_thread::sleep_for(10s);
            return true;
        }

        void refresh() override;
    protected:
        boost::mysql::tcp_connection connection_;
    };

    inline void mysql_connection::refresh() {
    }
} // obelisk::database::mysql

#endif //MYSQL_CONNECTION_H
