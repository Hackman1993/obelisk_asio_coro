#ifndef OBELISK_REQUIRED_VALIDATOR_H
#define OBELISK_REQUIRED_VALIDATOR_H

#include "validator_base.h"
namespace obelisk::http::validator {

    class required_validator : public validator_base {
    public:
        boost::asio::awaitable<void> validate(const std::string &name, http_request_wrapper &request) override;
    };

    std::shared_ptr<required_validator> required();
}

#endif //OBELISK_REQUIRED_VALIDATOR_H
