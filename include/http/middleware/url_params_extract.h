//
// Created by Hackman.Lo on 2023/12/22.
//

#ifndef URL_PARAMS_EXTRACT_H
#define URL_PARAMS_EXTRACT_H
#include "middleware.h"

namespace obelisk::http::middleware {
    class url_params_extract: public before_middleware{
    public:
        url_params_extract() = default;
        boost::cobalt::task<std::unique_ptr<http_response>> pre_handle(http_request_wrapper& request) override;
    };
} // obelisk::http::middleware

#endif //URL_PARAMS_EXTRACT_H
