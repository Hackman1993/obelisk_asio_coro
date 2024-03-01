#include "core/details/acceptor_base.h"
#include <iostream>
#include <boost/cobalt/spawn.hpp>

namespace obelisk::core::details {
    acceptor_base::acceptor_base(boost::asio::io_context& ctx): acceptor_(ctx) {
    }

    acceptor_base::~acceptor_base() {
        std::cout << "acceptor_base Released" << std::endl;
    }

    void acceptor_base::listen(const std::string &addr, std::uint16_t port) {
        const boost::asio::ip::address address = boost::asio::ip::address::from_string(addr);
        const boost::asio::ip::tcp::endpoint ep(address, port);
        acceptor_.open(ep.protocol());
        acceptor_.bind(ep);
        acceptor_.listen();
        boost::cobalt::spawn(acceptor_.get_executor(), this->accept_(), boost::asio::detached);
    }

    boost::cobalt::task<void> details::acceptor_base::accept_() {
        auto executor = co_await boost::asio::this_coro::executor;
        while (true){
            auto socket = co_await acceptor_.async_accept(boost::cobalt::use_op);
            auto wrapped = e_accepted(socket);
        };
    }
} // details