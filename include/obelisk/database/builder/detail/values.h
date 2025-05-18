//
// Created by hackman on 5/18/25.
//

#ifndef VALUES_H
#define VALUES_H
#include <obelisk/database/core/common.h>
namespace obelisk::database::builder::detail
{

    class values_t : public core::base_statement{
    public:
        values_t() = default;
        values_t(std::initializer_list<core::sql_value> values);
        std::string compile() override;
        core::sql_value& emplace_back(core::sql_value value)
        {
            return values_.emplace_back(std::move(value));
        }
    private:
        std::vector<core::sql_value> values_;
    };

}

#endif //VALUES_H
