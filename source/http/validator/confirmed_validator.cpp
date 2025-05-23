//
// Created by Hackman.Lo on 2024/3/5.
//

#include "obelisk/http/validator/confirmed_validator.h"
#include <boost/lexical_cast.hpp>
#include <nlohmann/json.hpp>
#include <obelisk/http/exception/validation_exception.h>

#include "obelisk/http/core/http_request.h"
#include "obelisk/http/exception/http_exception.h"


namespace obelisk::http::validator
{
    boost::asio::awaitable<void> confirmed_validator::validate(const std::string& name, http_request_wrapper& request)
    {
        if (request.params().contains(name))
        {
            if (!request.params().contains(name + "_confirmation"))
                throw validation_exception("server.error.validation." + name + "_confirmation_required");
            auto val = request.params()[name].get<std::string>();
            auto val_confirmation = request.params()[name + "_confirmation"].get<std::string>();
            if (val != val_confirmation)
            {
                throw validation_exception("server.error.validation." + name + "_not_match");
            }
        }
        co_return;
    }
} // obelisk::http::validator
