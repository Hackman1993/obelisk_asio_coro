/***********************************************************************************************************************
 * @author Hackman Lo
 * @file http_connection.h
 * @description 
 * @created_at 2023-10-13
***********************************************************************************************************************/

#ifndef OBELISK_HTTP_CONNECTION_H
#define OBELISK_HTTP_CONNECTION_H
#include <queue>
#include "../core/details/socket_base.h"

namespace obelisk::http {
    class http_request;
    class http_response;
    class raw_http_response;
    class http_block_data;
    class http_connection : public core::details::socket_base {
    public:
        http_connection(boost::asio::ip::tcp::socket &socket);

        void write_response(const std::shared_ptr<http_response> &resp);
        void on_request_received(const std::function<std::shared_ptr<http_response>(std::shared_ptr<http_request>&)>& callback);
    protected:

        boost::asio::awaitable<void> handle_co_();
        virtual void e_request_received(std::shared_ptr<http_request>& request);
        std::atomic_bool expecting_body_ = false;
        std::shared_ptr<http_block_data> request_;
        std::uint64_t body_reamains_ = 0;
        std::queue<std::shared_ptr<http::raw_http_response>> out_requests_;
        std::function<std::shared_ptr<http_response> (std::shared_ptr<http_request>&)> f_requested_;
        bool handle_header_();
        bool handle_body_();
        void write_outgoing_request();
        void e_connected() override;
        void e_data_sent(std::size_t bytes_transferred) override;
        bool e_data_received(std::size_t bytes_transferred) override;
    };

} // obelisk::http

#endif //OBELISK_HTTP_CONNECTION_H
