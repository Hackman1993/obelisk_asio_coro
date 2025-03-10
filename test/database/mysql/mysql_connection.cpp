//
// Created by Hackman.Lo on 2024/7/11.
//

#include "mysql_connection.h"

void mysql_connection::refresh() {
}

bool mysql_connection::reset() {
    boost::mysql::error_code ec;
    boost::mysql::diagnostics diagnostics;
    reset_connection(ec, diagnostics);
    set_character_set(boost::mysql::character_set("utf8mb4"), ec, diagnostics);

    boost::mysql::throw_on_error(ec, diagnostics);
    return true;
}

mysql_connection::~mysql_connection() {
}
