//
// Created by Hackman.Lo on 2024/2/26.
//

#ifndef AWAITABLE_OPERATION_H
#define AWAITABLE_OPERATION_H
#include <boost/cobalt.hpp>

namespace obelisk::core::coroutine {
    template<typename ReturnType>
    struct awaitable_result {
        bool success_ = false;
        std::shared_ptr<ReturnType> result_;
        std::shared_ptr<std::exception> exception_;
    };

    template<typename ReturnType>
    struct thread_awaitable_operation : boost::cobalt::enable_awaitables<awaitable_result<ReturnType>> {
        virtual bool await_ready() const noexcept { return false; }

        virtual ~thread_awaitable_operation() = default;

        virtual void await_suspend(std::coroutine_handle<> coro){

            std::thread([&,coro]{
              try{
                receiver_.result = this->_handle();
                result_.success_ = true;
                coro.resume();
              }catch (std::exception& e){
                this->result_.exception_ = std::make_shared<std::exception>(e);
              }
            }).detach();
        };

        virtual awaitable_result<ReturnType> await_resume() const noexcept {
            return result_;
        };
    protected:
        virtual ReturnType _handle() = 0;
        boost::cobalt::detail::promise_receiver<ReturnType> receiver_;
        awaitable_result<ReturnType> result_;
    }; // struct awaitable_operation
} // obelisk::core::coroutine

#endif //AWAITABLE_OPERATION_H
