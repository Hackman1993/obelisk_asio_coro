/***********************************************************************************************************************
 * @author Hackman Lo
 * @file http_exception.h
 * @description 
 * @created_at 2023-10-10
***********************************************************************************************************************/

#ifndef OBELISK_HTTP_EXCEPTION_H
#define OBELISK_HTTP_EXCEPTION_H
#include <sahara/exception/exception_base.h>
#include "../core/http_response_code.h"
namespace obelisk::http {
    class http_exception : public sahara::exception::exception_base {
    public:
        http_exception(const std::string& message, EResponseCode code) : sahara::exception::exception_base(message), code_(code){}
        std::uint32_t code(){ return code_;};
    protected:
        EResponseCode code_;
    };

} // obelisk

#endif //OBELISK_HTTP_EXCEPTION_H
