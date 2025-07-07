//
// Created by hackman on 7/5/25.
//

#ifndef MYSQL_CONNECTION_NEW_H
#define MYSQL_CONNECTION_NEW_H
#include <boost/mysql/any_connection.hpp>
struct mysql_connection_params
{
    std::string host;
    std::string password;
    std::string user;
    std::uint16_t port;
    std::string database;
};

struct mysql_construct_params{};
class mysql_connection_new
{
    public:
    template <typename CompletionToken>
    auto async_pool_connect(const boost::mysql::connect_params& params, CompletionToken&& token)
    {
        // boost::system::error_code ec;
         boost::mysql::diagnostics diagnostics;
        // conn_.connect(params, ec, diagnostics);
        // if (ec)
        //     std::cout << ec.message() << std::endl;
        // connect(params, ec, diagnostics);
        // boost::mysql::throw_on_error(ec, diagnostics);

        return conn_.async_connect(params,diagnostics,token);
    }

    mysql_connection_new(boost::asio::io_context& ioctx, mysql_construct_params params = {}): conn_(ioctx){}


    boost::mysql::any_connection conn_;
};

#endif //MYSQL_CONNECTION_NEW_H
