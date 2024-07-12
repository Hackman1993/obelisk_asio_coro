#include "http/validator/file_validator.h"
#include <memory>
#include "http/core/http_request.h"
#include "http/exception/validation_exception.h"

namespace obelisk::http::validator {
    std::shared_ptr<file_validator> file(std::vector<std::string> &acceptable) {

        return std::make_shared<file_validator>(acceptable);
    }

    boost::asio::awaitable<void> file_validator::validate(const std::string &name, http_request_wrapper &request) {
        if (!request.filebag().contains(name))
            co_return;
        if(acceptable_extensions_.empty())
            co_return;
        const auto &extention = request.filebag()[name]->extension_;
        if(!extention.has_value()) {
            throw validation_exception("validator.error.unprocessable_extension");
        }

        for(auto &item : acceptable_extensions_) {
            if(item == extention.value()) {
                co_return;
            }
            throw validation_exception("validator.error.unprocessable_extension");
        }
        co_return;
    }
}
