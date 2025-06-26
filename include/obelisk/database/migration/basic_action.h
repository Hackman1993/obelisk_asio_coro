//
// Created by hackman on 4/24/25.
//

#ifndef BASIC_ACTION_H
#define BASIC_ACTION_H
#include <string>
#include <utility>

namespace obelisk::database::migration::detail
{
    class basic_action
    {
    public:
        explicit basic_action(std::string sql, const bool required = true): sql_(std::move(sql)), required_(required){};
        [[nodiscard]] const std::string& sql() const
        {
            return sql_;
        };
        [[nodiscard]] bool required() const
        {
            return required_;
        }
    protected:
        std::string sql_;
        bool required_;
    };
}
#endif //BASIC_ACTION_H
