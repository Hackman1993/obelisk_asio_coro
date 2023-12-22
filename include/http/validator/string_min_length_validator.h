#ifndef OBELISK_STRING_MIN_LENGTH_VALIDATOR_H
#define OBELISK_STRING_MIN_LENGTH_VALIDATOR_H
#include "validator_base.h"
namespace obelisk::http::validator {

    class string_min_length_validator : public validator_base{
    public:
      string_min_length_validator(std::size_t length): length_(length){}
      void validate(const std::string &name, http_request_wrapper &request) override;
    protected:
      std::size_t length_ = 0;
    public:
    };

    std::shared_ptr<string_min_length_validator> min_length(std::size_t len);

} // obelisk::http

#endif //OBELISK_STRING_MIN_LENGTH_VALIDATOR_H
