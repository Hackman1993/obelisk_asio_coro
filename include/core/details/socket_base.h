//
// Created by Hackman.Lo on 10/12/2023.
//

#ifndef OBELISK_SOCKET_BASE_H
#define OBELISK_SOCKET_BASE_H
#include <memory>
#include <boost/asio.hpp>

namespace obelisk::core::details {

    class socket_base : public std::enable_shared_from_this<socket_base>{
    public:
        virtual void connect(const std::string& addr, std::uint16_t port);
    protected:
        virtual ~socket_base();
        virtual void e_connected() = 0;
        virtual void e_data_sent(std::size_t bytes_transferred) = 0;
        virtual bool e_data_received(std::size_t bytes_transferred) = 0;
        virtual void send();
        virtual void close();
    protected:
        explicit socket_base(boost::asio::ip::tcp::socket& socket);
        explicit socket_base(boost::asio::io_context& ctx);
        std::atomic_int8_t io_reference_;
        std::atomic_bool write_pending_ = false;
        boost::asio::streambuf instream_;
        boost::asio::streambuf outstream_;
        boost::asio::ip::tcp::socket socket_;
        std::shared_ptr<socket_base> self_holder_;
        void post_receive();
        void bytes_received_(const boost::system::error_code& error, std::size_t bytes_transferred);
        void bytes_sent_(const boost::system::error_code& error, std::size_t bytes_transferred);
        void e_connected_();
        virtual void e_disconnected(){};

        friend class acceptor_base;
    };

} // obelisk::core::details

#endif //OBELISK_SOCKET_BASE_H
