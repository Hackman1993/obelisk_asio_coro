/***********************************************************************************************************************
 * @author Hackman Lo
 * @file middleware.h
 * @description 
 * @created_at 2023-10-10
***********************************************************************************************************************/

#ifndef OBELISK_MIDDLEWARE_H
#define OBELISK_MIDDLEWARE_H
#include <memory>

#include "http/core/http_request.h"
#include "http/core/http_response.h"

namespace obelisk::http::middleware {

    class http_middleware_base{
    public:
        http_middleware_base();
        virtual ~http_middleware_base() = default;
    };
    class before_middleware: public http_middleware_base {
    public:
        virtual std::shared_ptr<http_response> pre_handle(http_request_wrapper& request) = 0;
    };

    class after_middleware: public http_middleware_base {
    public:
        virtual void after_handle(http_request_wrapper& request, std::shared_ptr<http_response> response) = 0;
    };

} // obelisk

#endif //OBELISK_MIDDLEWARE_H
