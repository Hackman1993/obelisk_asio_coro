//
// Created by Hackman.Lo on 2023/12/26.
//

#ifndef DB_CONNECTION_BASE_H
#define DB_CONNECTION_BASE_H
#include <boost/asio/awaitable.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace obelisk::database {
    class db_connection_base {
    public:
        /**
         * @brief Use to refresh connection when holding a long time connection
         */
        virtual void refresh() = 0;

        /**
         * @brief Use to reset connection if connection is stateful
         */
        virtual bool reset() = 0;
        virtual ~db_connection_base() = default;

        [[nodiscard]] bool reuse() const { return reuse_; }
        void reuse(bool r) { reuse_ = r; }
    protected:
        bool reuse_ = false;
    };
} // obelisk::database

#endif //DB_CONNECTION_BASE_H
