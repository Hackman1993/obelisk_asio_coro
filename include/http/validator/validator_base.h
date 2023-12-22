#ifndef OBELISK_VALIDATOR_BASE_H
#define OBELISK_VALIDATOR_BASE_H

#include <memory>
#include <string>
#include <vector>

namespace obelisk::http {
    class http_request_wrapper;
}

namespace obelisk::http::validator {
    class http_request;
        class validator_base {
        public:
            virtual ~validator_base() = default;
            virtual void validate(const std::string &name, http_request_wrapper &request) = 0;
        };

        struct validator_group {
            std::string name_;
            std::vector<std::shared_ptr<validator_base>> validators_;
        };
} // validator

#endif //OBELISK_VALIDATOR_BASE_H
