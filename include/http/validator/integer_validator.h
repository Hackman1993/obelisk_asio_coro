//
// Created by Hackman.Lo on 2024/3/5.
//

#ifndef INTEGER_VALIDATOR_H
#define INTEGER_VALIDATOR_H
#include <memory>

#include "validator_base.h"

namespace obelisk::http::validator {
    class integer_validator final: public validator_base {
    public:
        integer_validator(bool is_signed) : signed_(is_signed) {}
        boost::cobalt::task<void> validate(const std::string &name, http_request_wrapper &request) override;
    private:
        bool signed_;
    };

    std::shared_ptr<integer_validator> integer(bool is_signed = true);

    inline std::shared_ptr<integer_validator> integer(bool is_signed) {
        return std::make_shared<integer_validator>(is_signed);
    }
} // obelisk::http::validator

#endif //INTEGER_VALIDATOR_H
