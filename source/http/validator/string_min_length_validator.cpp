#include "http/validator/string_min_length_validator.h"
#include "http/exception/validation_exception.h"
#include "http/core/http_request.h"

namespace obelisk::http::validator {
    boost::cobalt::task<void> string_min_length_validator::validate(const std::string&name,
                                                                    http_request_wrapper&request) {
        if (request.params().contains(name)) {
            const auto&value = request.params()[std::string(name)];
            auto success = true;
            if (
                (value.is_string() && value.as_string().size() < length_) ||
                (value.is_int64() && value.as_int64() < length_) ||
                (value.is_uint64() && value.as_uint64() < length_) ||
                (value.is_array() && value.as_uint64() < length_) ||
                (value.is_double() && value.as_double() < static_cast<double>(length_)))
                success = false;
            else if (value.is_bool() || value.is_object() || value.is_null()) {
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
