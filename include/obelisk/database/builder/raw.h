//
// Created by hackman on 4/27/25.
//

#ifndef RAW_H
#define RAW_H
#include <utility>

#include "base_statement.h"

class raw : public base_statement
{
public:
    raw() = default;
    raw(std::string sql) : sql_(std::move(sql)) {}
    std::string compile() override
    {
        return sql_;
    };

private:
    std::string sql_;
};

#endif //RAW_H
