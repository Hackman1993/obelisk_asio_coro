
#include "http/validator/string_min_length_validator.h"
#include "http/exception/validation_exception.h"
#include "http/core/http_request.h"
namespace obelisk::http::validator {

    boost::asio::awaitable<void> string_min_length_validator::validate(const std::string &name, http_request_wrapper &request){
        if (request.params().contains(name)) {
            if(const auto &value = request.params()[std::string(name)];value.size() == 1 && value[0].length() < length_){
                throw obelisk::http::validation_exception("Param " + name + " length must be greater than " + std::to_string(length_));
            }
        }
        co_return;
    }

    std::shared_ptr <string_min_length_validator> min_length(std::size_t len){
        return std::make_shared<string_min_length_validator>(len);
    };
}

