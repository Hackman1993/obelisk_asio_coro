//
// Created by Hackman.Lo on 2023/12/28.
//

#ifndef ASYNC_MUTEX_H
#define ASYNC_MUTEX_H
#include <boost/asio/basic_io_object.hpp>
#include <boost/asio/io_service.hpp>

namespace obelisk::core::coroutine {

    template <typename Service>
    class basic_async_mutex : public boost::asio::basic_io_object<Service> {
    public:
        explicit basic_async_mutex(boost::asio::io_context& context): boost::asio::basic_io_object<Service>(context){}

        template <typename Handler>
        void async_lock() {

        }
    };

    template <typename AsyncMutexImpl>
    class basic_async_mutex_service : public boost::asio::io_service::service {
    public:
        static boost::asio::io_service::id id;

        explicit basic_async_mutex_service(boost::asio::io_service &io_service) : boost::asio::io_service::service(io_service){}

        typedef boost::shared_ptr<AsyncMutexImpl> implementation_type;
        void construct(implementation_type& impl) {
            impl.reset(new AsyncMutexImpl());
        }

        void destroy(implementation_type& impl) {
            impl->destroy();
            impl.reset();
        }
    private:
        void shutdown_service(){}
    };

    template <typename TimerImplementation>
    boost::asio::io_service::id basic_async_mutex_service<TimerImplementation>::id;

    class async_mutex_service_impl {
    public:
        async_mutex_service_impl(){}
        void destroy(){};

    };

    typedef basic_async_mutex<basic_async_mutex_service<async_mutex_service_impl>> async_mutex;
} // obelisk::core::coroutine

#endif //ASYNC_MUTEX_H
