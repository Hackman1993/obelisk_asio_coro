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
        if(request.additional_data().contains("__json_body")) {
            const auto& json_data = std::any_cast<boost::json::object>(request.additional_data()["__json_body"]);
            if(json_data.contains(name)) {
                co_return;
            }
        }
        throw validation_exception("validation.error." + name + "_required");
    }
}
