#include "http/validator/required_validator.h"
#include <memory>
#include <boost/json.hpp>
#include "http/core/http_request.h"
#include "http/exception/validation_exception.h"

namespace obelisk::http::validator {
    std::shared_ptr<required_validator> required() {
        return std::make_shared<required_validator>();
    }

    boost::cobalt::task<void> required_validator::validate(const std::string&name, http_request_wrapper&request) {
        if (request.params().contains(name) || request.filebag().contains(name)) {
            co_return;
        }
        throw validation_exception("validation.error." + name + "_required");
    }
}
