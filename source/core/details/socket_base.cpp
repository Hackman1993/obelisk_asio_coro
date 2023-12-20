#include <iostream>
#include "core/details/socket_base.h"
#include <sahara/log/log.h>

namespace obelisk::core::details {
    void socket_base::e_connected_() {
        self_holder_ = shared_from_this();
        post_receive();
        e_connected();
    }

    void socket_base::send() {
        io_reference_ += 1;
        write_pending_ = true;
        boost::asio::async_write(socket_, outstream_, [&](const boost::system::error_code &error, std::size_t bytes_transferred){
            io_reference_ -= 1;
            write_pending_ = false;
            if(bytes_transferred > 0) outstream_.consume(bytes_transferred);
            bytes_sent_(error, bytes_transferred);

        });
    }

    void socket_base::bytes_received_(const boost::system::error_code &error, std::size_t bytes_transferred) {
        if(error){
            if(error != boost::asio::error::eof)
                std::cout << error.what() << error.value() << std::endl;
            close();
            return;
        }
        instream_.commit(bytes_transferred);
        if(e_data_received(bytes_transferred)) post_receive();
        else this->close();
    }

    void socket_base::bytes_sent_(const boost::system::error_code &error, std::size_t bytes_transferred) {
        if(error){
            if(error != boost::asio::error::eof)
                std::cout << error.what() << error.value() << std::endl;
            close();
            return;
        }
        e_data_sent(bytes_transferred);
    }

    void socket_base::close() {
        if(socket_.is_open()){
            socket_.close();
        }
        if(io_reference_ == 0){
            e_disconnected();
            self_holder_ = nullptr;
        }
    }

    void socket_base::post_receive() {
        io_reference_ += 1;
        socket_.async_receive(instream_.prepare(256 * 1024), [&](const boost::system::error_code &error, std::size_t bytes_transferred){
            io_reference_ -= 1;
            bytes_received_(error, bytes_transferred);
        });
    }

    socket_base::~socket_base() {
        std::cout << "Base Socket Released" << std::endl;
        if(socket_.is_open())
            socket_.close();
    }

    void socket_base::connect(const std::string &addr, std::uint16_t port) {
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(addr),port);
        socket_.async_connect(ep, [&](const boost::system::error_code& error){
            if(!error)
                this->e_connected_();
            else{
                LOG_DEBUG("Socket connection failed due to :{}", error.what());
            }
        });

    }

    socket_base::socket_base(boost::asio::io_context &ctx) : socket_(ctx){}
    socket_base::socket_base(boost::asio::ip::tcp::socket &socket): socket_(std::move(socket)) {}
} // obelisk::core::details