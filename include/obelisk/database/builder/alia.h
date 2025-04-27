//
// Created by hackman on 4/25/25.
//

#ifndef COLUMN_NAMES_H
#define COLUMN_NAMES_H
#include <optional>
#include <string>

#include "base_statement.h"

class alia :public base_statement
{
public:
    alia(std::string name): name_(std::move(name)){ }
    alia(const char* name): name_(name){}
    alia(std::initializer_list<std::string> name)
    {

    }

    alia& operator = (const char* name)
    {
        name_ = name;
        return *this;
    }

    alia& operator = (const std::string& name)
    {
        name_ = name;
        return *this;
    }

    alia(std::string name, const std::string& alias): name_(std::move(name)), alias_(" AS " + alias) { }

    std::string compile() override
    {
        std::string result = name_;
        if (alias_)
            result.append(alias_.value());
        return result;
    }

protected:
    std::string name_;
    std::optional<std::string> alias_;
};

#endif //COLUMN_NAMES_H
