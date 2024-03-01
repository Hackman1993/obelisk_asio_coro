//
// Created by Hackman.Lo on 2024/2/26.
//

#ifndef AWAITABLE_OPERATION_H
#define AWAITABLE_OPERATION_H
#include <boost/cobalt/concepts.hpp>

namespace obelisk::core::coroutine {
        template<typename ReturnType>
        struct awaitable_operation : boost::cobalt::enable_awaitables<ReturnType> {
            virtual bool await_ready() const noexcept { return false; }

            virtual ~awaitable_operation() = default;

            virtual void await_suspend(std::coroutine_handle<> coro) = 0;

            virtual ReturnType await_resume() const noexcept =0;
        }; // struct awaitable_operation
} // obelisk::core::coroutine

#endif //AWAITABLE_OPERATION_H
