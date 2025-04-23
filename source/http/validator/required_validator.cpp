#include "obelisk/http/validator/required_validator.h"
#include <memory>
#include <boost/json.hpp>
#include "obelisk/http/core/http_request.h"
#include "obelisk/http/exception/validation_exception.h"
#include <boost/cobalt/task.hpp>
namespace obelisk::http::validator {
    std::shared_ptr<required_validator> required() {
        return std::make_shared<required_validator>();
    }

    obelisk::task<void> required_validator::validate(const std::string&name, http_request_wrapper&request) {
        if(request.filebag().contains(name))
            co_return;
        if (request.params().contains(name)) {
            if(!request.params()[name].is_string())
                co_return;

            if(!request.params()[name].as_string().empty()) {
                co_return;
            }
        }
        throw validation_exception("validation.error." + name + "_required");
    }
}
