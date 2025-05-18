//
// Created by hackman on 5/16/25.
//

#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "base_statement.h"
#include "col.h"
#include "condition.h"
#include "raw.h"

class aggregate_param_raw: public std::variant<raw, condition, col>, public base_statement
{
public:
    using std::variant<raw, condition, col>::variant;
    std::string compile() override
    {
        return std::visit(obelisk::database::builder::core::general_visitor{}, *this);
    }
};

class max final : public base_statement{
public:
    std::string compile() override{ return raw_.compile(); }
private:
    aggregate_param_raw raw_;
};



#endif //AGGREGATE_H
