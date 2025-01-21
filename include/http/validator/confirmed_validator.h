#ifndef OBELISK_CONFIRMED_VALIDATOR_H
#define OBELISK_CONFIRMED_VALIDATOR_H
#include "validator_base.h"

namespace obelisk::http::validator {

    class confirmed_validator : public validator_base{

    public:
      boost::cobalt::task<void> validate(const std::string &name, http_request_wrapper &request) override;
    };

    std::shared_ptr<confirmed_validator> confirmed();
} // obelisk::http::validator

#endif //OBELISK_CONFIRMED_VALIDATOR_H
