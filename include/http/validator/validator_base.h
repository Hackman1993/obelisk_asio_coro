#ifndef OBELISK_VALIDATOR_BASE_H
#define OBELISK_VALIDATOR_BASE_H

#include <memory>
#include <sahara/string/string.h>

namespace obelisk::http {
    class http_request;
        class validator_base {
        public:
            virtual void validate(const std::string &name, http_request &request) = 0;
        };

        struct validator_group {
            std::string name_;
            std::vector<std::shared_ptr<validator_base>> validators_;
        };
} // validator

#endif //OBELISK_VALIDATOR_BASE_H
