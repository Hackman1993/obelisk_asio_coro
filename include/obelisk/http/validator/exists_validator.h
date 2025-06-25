//
// Created by hackman on 5/27/25.
//

#ifndef EXISTS_VALIDATOR_H
#define EXISTS_VALIDATOR_H
#include <utility>

#include "validator_base.h"


namespace obelisk::http::validator {

    class exists_validator final : public validator_base{
    public:
        exists_validator(std::string  table, std::string  column, const bool soft_delete): table_(std::move(table)), column_(std::move(column)), soft_delete_(soft_delete){};
        boost::asio::awaitable<void> validate(const std::string& name, http_request_wrapper& request) override;

    private:
        std::string table_;
        std::string column_;
        bool soft_delete_;
    };
    inline std::shared_ptr<exists_validator> exists(const std::string& table, const std::string& column = "id", bool soft_delete = true) {
        return std::make_shared<exists_validator>(table, column, soft_delete);
    }

} // obelisk::http::validator

#endif //EXISTS_VALIDATOR_H
