//
// Created by Hackman.Lo on 2024/3/5.
//

#include "obelisk/http/validator/array_validator.h"
#include <boost/lexical_cast.hpp>
#include <nlohmann/json.hpp>
#include <obelisk/http/exception/validation_exception.h>

#include "obelisk/http/core/http_request.h"
#include "obelisk/http/exception/http_exception.h"


namespace obelisk::http::validator
{
    boost::asio::awaitable<void> array_validator::validate(const std::string& name, http_request_wrapper& request)
    {
        if (request.params().contains(name))
        {
          if(!request.params()[name].is_array())
              throw http_exception(std::format("server.error.{}_must_be_array", name), EST_UNPROCESSABLE_CONTENT);
        }
        co_return;
    }
} // obelisk::http::validator
