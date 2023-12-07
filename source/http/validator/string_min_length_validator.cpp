
#include "http/validator/string_min_length_validator.h"
#include "http/exception/validation_exception.h"
#include "http/core/http_request.h"
namespace obelisk::http {

    void string_min_length_validator::validate(const std::string &name, http_request &request){
        if (request.contains_param(name)) {
            auto value = request.param(name);
            if(value.size() == 1 && value[0].length() < length_){
                throw obelisk::http::validation_exception("Param " + name + " length must be greater than " + std::to_string(length_));
            }
        }
    }

    std::shared_ptr <obelisk::http::string_min_length_validator> min_length(std::size_t len){
        return std::make_shared<obelisk::http::string_min_length_validator>(len);
    };
}

