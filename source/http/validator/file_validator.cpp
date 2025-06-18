#include "obelisk/http/validator/file_validator.h"
#include <memory>
#include "obelisk/http/core/http_request.h"
#include "obelisk/http/exception/validation_exception.h"

namespace obelisk::http::validator {
    std::shared_ptr<validator_base> file(const std::initializer_list<std::string>& acceptable) {
        return std::make_shared<file_validator>(acceptable);
    }

    boost::asio::awaitable<void> file_validator::validate(const std::string &name, http_request_wrapper &request) {
        if (!request.filebag().contains(name))
            co_return;
        if(acceptable_mimes_.empty())
            co_return;
        const auto &mime = request.filebag()[name]->mime_type_;

        if (acceptable_mimes_.contains(mime))
            co_return;
        throw validation_exception("validator.error.unprocessable_extension");
    }
}
