#ifndef OBELISK_ARRAY_VALIDATOR_H
#define OBELISK_ARRAY_VALIDATOR_H
#include "validator_base.h"

namespace obelisk::http::validator {

    class array_validator : public validator_base{

    public:
        boost::asio::awaitable<void> validate(const std::string &name, http_request_wrapper &request) override;
    };

    inline std::shared_ptr<array_validator> array(){ return std::make_shared<array_validator>();}
} // obelisk::http::validator

#endif //OBELISK_ARRAY_VALIDATOR_H
