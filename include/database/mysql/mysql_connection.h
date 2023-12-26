//
// Created by Hackman.Lo on 2023/12/26.
//

#ifndef MYSQL_CONNECTION_H
#define MYSQL_CONNECTION_H
#include "../db_connection_base.h"
namespace obelisk::database::mysql {
    class mysql_connection : public db_connection_base{
    public:
        mysql_connection() = default;

        void refresh() override;
    };

    inline void mysql_connection::refresh() {
    }
} // obelisk::database::mysql

#endif //MYSQL_CONNECTION_H
