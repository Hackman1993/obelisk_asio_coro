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
    class protocol_exception : public std::logic_error {
    public:
        explicit protocol_exception(const std::string& message) : std::logic_error(message){}
    };

} // obelisk

#endif //OBELISK_PROTOCOL_EXCEPTION_H
