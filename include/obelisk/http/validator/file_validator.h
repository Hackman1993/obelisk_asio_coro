//
// Created by Hackman.Lo on 2024/2/23.
//

#ifndef FILE_VALIDATOR_H
#define FILE_VALIDATOR_H
#include <algorithm>
#include <map>
#include "validator_base.h"

namespace obelisk::http::validator {
    class file_validator : public validator_base {
    public:
        file_validator(const std::initializer_list<std::string>& acceptable){
            std::ranges::for_each(acceptable, [this](const auto& key) {
                acceptable_mimes_.emplace(key, true);
            });
        }
        obelisk::task<void> validate(const std::string &name, http_request_wrapper &request) override;
    private:
        std::unordered_map<std::string, bool> acceptable_mimes_;
    };

    std::shared_ptr<validator_base> file(const std::initializer_list<std::string>& acceptable = {});
} // obelisk

#endif //FILE_H
