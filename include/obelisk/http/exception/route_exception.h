/***********************************************************************************************************************
 * @author Hackman Lo
 * @file route_exception.h
 * @description 
 * @created_at 2023-10-09
***********************************************************************************************************************/

#ifndef OBELISK_ROUTE_EXCEPTION_H
#define OBELISK_ROUTE_EXCEPTION_H
#include <sahara/exception/exception_base.h>
namespace obelisk {

    class route_exception : public sahara::exception::exception_base {
    public:
        route_exception(const std::string& message) : sahara::exception::exception_base(message){}
    };

} // obelisk

#endif //OBELISK_ROUTE_EXCEPTION_H
