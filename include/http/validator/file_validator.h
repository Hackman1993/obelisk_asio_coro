//
// Created by Hackman.Lo on 2024/2/23.
//

#ifndef FILE_VALIDATOR_H
#define FILE_VALIDATOR_H
#include "validator_base.h"

namespace obelisk::http::validator {
    class file_validator : public validator_base {
    public:
        file_validator(std::vector<std::string> acceptable): acceptable_extensions_(std::move(acceptable)) {

        }
        boost::asio::awaitable<void> validate(const std::string &name, http_request_wrapper &request) override;
    private:
        std::vector<std::string> acceptable_extensions_;
    };

    std::shared_ptr<file_validator> file();
} // obelisk

#endif //FILE_H
