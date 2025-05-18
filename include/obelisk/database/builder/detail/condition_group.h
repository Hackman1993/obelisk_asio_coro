//
// Created by hackman on 4/27/25.
//

#ifndef WHERE_GROUP_H
#define WHERE_GROUP_H
#include "condition.h"
#include "utils.h"

namespace obelisk::database::builder::detail
{
    class condition_group : public core::base_statement
    {
    public:
        condition_group() = default;

        using value_type = std::vector<condition>::value_type;
        void push_back(condition condition)
        {
            conditions_.push_back(std::move(condition));
        }

        std::string compile() override
        {
            return std::format("({})",utils::separate_with(conditions_, " AND "));
        }

        [[nodiscard]] auto size() const
        {
            return conditions_.size();
        }

        [[nodiscard]] bool empty() const
        {
            return conditions_.empty();
        }

        auto begin()
        {
            return conditions_.begin();
        }
        auto end()
        {
            return conditions_.end();
        }
    private:
        std::vector<condition> conditions_;
    };
}
#endif //WHERE_GROUP_H
