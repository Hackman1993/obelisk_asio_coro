//
// Created by Hackman.Lo on 2024/3/5.
//

#include "obelisk/http/validator/integer_validator.h"

#include <boost/lexical_cast.hpp>
#include <nlohmann/json.hpp>
#include "obelisk/http/core/http_request.h"
#include "obelisk/http/exception/http_exception.h"


namespace obelisk::http::validator {
    boost::asio::awaitable<void> integer_validator::validate(const std::string&name, http_request_wrapper&request) {
        if(!request.params().contains(name))
            co_return;
        if(request.params()[name].is_string()) {
            try {
                if(signed_)
                    request.params()[name] = strtoll(request.params()[name].get<std::string>().c_str(),nullptr, 10);
                else
                    request.params()[name] = strtoull(request.params()[name].get<std::string>().c_str(), nullptr, 10);
            }catch (const std::exception& ) {
                throw http_exception("error.validator." + std::string(signed_? "integer":"unsigned_integer"), EST_UNPROCESSABLE_CONTENT);
            }
        }else if (request.params()[name].is_number_integer()) {
            if (!signed_)
            {
                if (request.params()[name].get<std::int64_t>() < 0)
                    throw http_exception("error.validator.unsigned_integer", EST_UNPROCESSABLE_CONTENT);
                request.params()[name] = static_cast<std::uint64_t>(request.params()[name].get<std::int64_t>());
            }
        }else if (request.params()[name].is_number_unsigned())
        {
            if (signed_)
            {
                if (request.params()[name].get<std::uint64_t>() > std::numeric_limits<std::int64_t>::max())
                    throw http_exception("error.validator.integer", EST_UNPROCESSABLE_CONTENT);
                request.params()[name] = static_cast<std::int64_t>(request.params()[name].get<std::uint64_t>());
            }
        }
        co_return;
    }
} // obelisk::http::validator
