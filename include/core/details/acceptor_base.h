#ifndef OBELISK_ACCEPTOR_BASE_H
#define OBELISK_ACCEPTOR_BASE_H
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/cobalt/task.hpp>

#include "socket_base.h"

namespace obelisk::core::details {
    class acceptor_base {
    public:
        explicit acceptor_base(boost::asio::io_context& ctx);
        virtual ~acceptor_base();
        void listen (const std::string &addr, std::uint16_t port);
    protected:
        boost::cobalt::task<void> accept_();

        boost::asio::ip::tcp::acceptor acceptor_;
        virtual std::shared_ptr<socket_base> e_accepted(boost::asio::ip::tcp::socket& socket) = 0;
    };
} // details

#endif //OBELISK_ACCEPTOR_BASE_H
