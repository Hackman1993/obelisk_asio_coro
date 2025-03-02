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
#include <stdexcept>
namespace obelisk::http {
    class http_exception : public std::logic_error {
    public:
        http_exception(const std::string& message, EResponseCode code) : std::logic_error(message), code_(code){}
        [[nodiscard]] EResponseCode code() const { return code_;};
    protected:
        EResponseCode code_;
    };

} // obelisk

#endif //OBELISK_HTTP_EXCEPTION_H
