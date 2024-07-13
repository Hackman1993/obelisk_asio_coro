/***********************************************************************************************************************
 * @author Hackman Lo
 * @file acceptor.h
 * @description 
 * @created_at 2023-10-13
***********************************************************************************************************************/

#ifndef OBELISK_ACCEPTOR_H
#define OBELISK_ACCEPTOR_H
#include "details/acceptor_base.h"
namespace obelisk {
    namespace core {

        template <typename T>
        class acceptor : public details::acceptor_base{
        public:
            explicit acceptor(boost::asio::io_context& ctx): details::acceptor_base(ctx){}

            void on_accepted(std::function<void (std::shared_ptr<T>)> callback){
                accepted_ = callback;
            };
        private:
            std::shared_ptr<details::socket_base> e_accepted(boost::asio::ip::tcp::socket &socket) override {
                auto ret = std::make_shared<T>(socket);
                if(ret && accepted_)
                    accepted_(ret);
                return ret;
            }
            std::function<void (std::shared_ptr<T>)> accepted_;

        };

    } // obelisk
} // core

#endif //OBELISK_ACCEPTOR_H
