//
// Created by Hackman.Lo on 2024/3/5.
//

#include "http/validator/integer_validator.h"

#include <boost/lexical_cast.hpp>

#include "http/core/http_request.h"
#include "http/exception/http_exception.h"


namespace obelisk::http::validator {
    boost::asio::awaitable<void> integer_validator::validate(const std::string&name, http_request_wrapper&request) {
        if(!request.params().contains(name))
            co_return;
        if(request.params()[name].is_string()) {
            try {
                if(signed_)
                    request.params()[name] = strtoll(request.params()[name].as_string().c_str(),nullptr, 10);
                else
                    request.params()[name] = strtoull(request.params()[name].as_string().c_str(), nullptr, 10);
            }catch (const std::exception& ) {
                throw http_exception("error.validator." + std::string(signed_? "integer":"unsigned_integer"), EST_UNPROCESSABLE_CONTENT);
            }
        }else if(request.params()[name].is_uint64() && !signed_) {
            co_return;
        }else if(request.params()[name].is_int64() && signed_) {
            co_return;
        }else {
            throw http_exception("error.validator." + std::string(signed_? "integer":"unsigned_integer"), EST_UNPROCESSABLE_CONTENT);
        }
        co_return;
    }
} // obelisk::http::validator
