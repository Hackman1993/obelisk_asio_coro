//
// Created by hackman on 5/17/25.
//

#ifndef INSERT_STATEMENT_H
#define INSERT_STATEMENT_H
#include "detail/col.h"
#include "detail/common.h"
#include "detail/sub_query.h"
#include "detail/table.h"
#include "detail/values.h"

namespace obelisk::database::builder
{

    class insert_statement : public detail::base_builder_statement
    {
    public:
        insert_statement() = default;
        insert_statement(const std::string& name): table_(name) {}
        insert_statement(const std::string& name, std::vector<detail::col> cols): table_(name), insert_columns_(std::move(cols)) {}
        std::string compile() override;
        insert_statement& into(const detail::table& tb);
        insert_statement& cols(std::vector<detail::col> columns)
        {
            insert_columns_ = std::move(columns);
            return *this;
        }

        insert_statement& ignore(bool val = true)
        {
            ignore_ = val;
            return *this;
        }
        insert_statement& values(const std::vector<std::pair<detail::col, detail::sql_value>>& value_pack)
        {
            insert_columns_.clear();

            detail::values_t t;
            for (auto&[first, second]: value_pack)
            {
                insert_columns_.emplace_back(first);
                t.emplace_back(second);
            }
            insert_values_.emplace_back(t);
            return *this;
        }

        insert_statement& values(std::vector<detail::col> columns, std::vector<detail::values_t> values)
        {
            insert_columns_ = std::move(columns);
            insert_values_ = std::move(values);
            return *this;
        }

        insert_statement& values(const detail::sub_query& sub_query)
        {
            sub_query_ = sub_query;
            return *this;
        }

    private:
        detail::table table_;
        bool ignore_ = false;
        std::optional<detail::sub_query> sub_query_;
        std::vector<detail::col> insert_columns_;
        std::vector<detail::values_t> insert_values_;
    };

}

#endif //INSERT_STATEMENT_H
