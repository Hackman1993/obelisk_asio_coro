//
// Created by Hackman.Lo on 2023/12/26.
//

#ifndef DB_CONNECTION_BASE_H
#define DB_CONNECTION_BASE_H

namespace obelisk::database {
    class db_connection_base {
    public:
        virtual void refresh() = 0;
        virtual ~db_connection_base() = 0;
    };
} // obelisk::database

#endif //DB_CONNECTION_BASE_H
