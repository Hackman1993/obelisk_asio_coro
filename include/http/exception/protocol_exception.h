/***********************************************************************************************************************
 * @author Hackman Lo
 * @file protocol_exception.h
 * @description 
 * @created_at 2023-08-30
***********************************************************************************************************************/

#ifndef OBELISK_PROTOCOL_EXCEPTION_H
#define OBELISK_PROTOCOL_EXCEPTION_H

#include <sahara/exception/exception_base.h>

namespace obelisk::http {
    class protocol_exception : public sahara::exception::exception_base {
    public:
        protocol_exception(const std::string& message) : sahara::exception::exception_base(message){}
    };

} // obelisk

#endif //OBELISK_PROTOCOL_EXCEPTION_H
