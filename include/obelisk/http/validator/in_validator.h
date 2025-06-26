#ifndef OBELISK_IN_VALIDATOR_H
#define OBELISK_IN_VALIDATOR_H

#include <map>
#include <nlohmann/json.hpp>
#include <obelisk/http/core/http_request.h>
#include <obelisk/http/exception/http_exception.h>

#include "validator_base.h"
namespace obelisk::http::validator {

    template <typename T>
    class in_validator final : public validator_base {
    public:
        explicit in_validator(const std::vector<T>& values)
        {
            for (auto& value: values)
                values_.emplace(value, true);
        }
        boost::asio::awaitable<void> validate(const std::string &name, http_request_wrapper &request) override
        {
            if (request.params().contains(name))
            {
                auto &params_val = request.params()[name];
                const T& val = params_val.get<T>();
                if (!values_.contains(val))
                    throw http_exception("server.error.value_not_in_set", EST_UNPROCESSABLE_CONTENT);
                co_return;
            }
        }
    private:
        std::map<T, bool> values_;
    };

    template <typename T>
    std::shared_ptr<in_validator<T>> in(const std::vector<T>& values)
    {
        return std::make_shared<in_validator<T>>(values);
    }
}

#endif //OBELISK_IN_VALIDATOR_H
