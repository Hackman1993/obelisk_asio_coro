#include "obelisk/http/validator/string_min_length_validator.h"
#include "obelisk/http/exception/validation_exception.h"
#include "obelisk/http/core/http_request.h"
#include <boost/cobalt.hpp>
#include <nlohmann/json.hpp>

namespace obelisk::http::validator {
    obelisk::task<void> string_min_length_validator::validate(const std::string&name,
                                                                    http_request_wrapper&request) {
        if (request.params().contains(name)) {
            const auto&value = request.params()[name];
            auto success = true;
            if (
                (value.is_string() && value.get<std::string>().size() < length_) ||
                (value.is_number_integer() && value.get<std::int64_t>() < length_) ||
                (value.is_number_unsigned() && value.get<std::uint64_t>() < length_) ||
                (value.is_array() && value.size() < length_) ||
                (value.is_number_float() && value.get<double>() < static_cast<double>(length_)))
                success = false;
            else if (value.is_boolean() || value.is_object() || value.is_null()) {
                throw validation_exception("validation.error.invalid_type[" + name + "]");
            }
            if (!success)
                throw validation_exception("validation.error.validation_error[" + name + "]");
        }
        co_return;
    }

    std::shared_ptr<string_min_length_validator> min_length(std::size_t len) {
        return std::make_shared<string_min_length_validator>(len);
    };
}
