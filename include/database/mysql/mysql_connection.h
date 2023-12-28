//
// Created by Hackman.Lo on 2023/12/26.
//

#ifndef MYSQL_CONNECTION_H
#define MYSQL_CONNECTION_H
#include "../db_connection_base.h"
#include <boost/mysql.hpp>
namespace obelisk::database::mysql {
    class mysql_connection : public db_connection_base{
    public:
        mysql_connection() = default;
        bool connect(const std::string& username, const std::string& password, const std::string& host) {
            return true;
        }

        bool reset() override{ return true;};

        bool execute() {
            using namespace  std::chrono_literals;
            std::this_thread::sleep_for(10s);
            return true;
        }

        void refresh() override;
    protected:

    };

    inline void mysql_connection::refresh() {
    }
} // obelisk::database::mysql

#endif //MYSQL_CONNECTION_H
