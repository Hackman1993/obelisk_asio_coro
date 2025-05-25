//
// Created by hackman on 5/23/25.
//

#ifndef IF_FORMAT_H
#define IF_FORMAT_H
#include "common.h"
#include "enable_where_condition.h"

namespace obelisk::database::builder::detail
{

    class if_format  :public enable_where_condition<if_format, false>, public base_statement{
    public:
        if_format(sql_value true_val, sql_value false_val):true_val_(std::move(true_val)), false_val_(std::move(false_val)){}
        std::string compile() override
        {
            return std::format("IF({} , {}, {}) ", compile_where(), true_val_.compile(), false_val_.compile());
        }

    private:
        sql_value true_val_;
        sql_value false_val_;
    };

}

#endif //IF_FORMAT_H
