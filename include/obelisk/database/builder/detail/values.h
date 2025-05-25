//
// Created by hackman on 5/18/25.
//

#ifndef VALUES_H
#define VALUES_H
#include "common.h"
namespace obelisk::database::builder::detail
{

    class values_t : public base_statement{
    public:
        values_t() = default;
        values_t(std::initializer_list<sql_value> values);
        std::string compile() override;
        sql_value& emplace_back(sql_value value)
        {
            return values_.emplace_back(std::move(value));
        }
    private:
        std::vector<sql_value> values_;
    };

}

#endif //VALUES_H
