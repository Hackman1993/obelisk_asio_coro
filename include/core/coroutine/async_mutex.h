#include <boost/asio.hpp>
class async_scoped_lock {
public:
    explicit async_scoped_lock(std::mutex& mutex): lock_(mutex){};
    template <typename CompletionToken>
    auto async_lock(CompletionToken&& token) {

        return boost::asio::async_compose<CompletionToken, void()>([&lock = this->lock_](auto&& self) {
           std::thread([self = std::move(self), &lock]() mutable {
               lock.lock();
               self.complete();
           }).detach();
       }, token);
    }

    ~async_scoped_lock() {
        lock_.unlock();
    }
protected:
    std::mutex& lock_;
};