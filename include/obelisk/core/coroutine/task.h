//
// Created by hackman on 2/28/25.
//

#ifndef TASK_H
#define TASK_H
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>

namespace obelisk::core::coroutine {
    template <typename T, typename Executor = boost::asio::any_io_executor>
    using task = boost::asio::awaitable<T, Executor>;
    template <typename Executor = boost::asio::any_io_executor>
    using use_token = boost::asio::use_awaitable_t<Executor>;
}
namespace obelisk {
    template <typename T, typename Executor = boost::asio::any_io_executor>
    using task = boost::asio::awaitable<T, Executor>;


    inline constexpr boost::asio::use_awaitable_t<> use_token(__FILE__, __LINE__, 0);

}
#endif //TASK_H
