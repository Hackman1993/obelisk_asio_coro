//
// Created by hackman on 5/18/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_QUERY_H
#define OBELISK_DATABASE_BUILDER_DETAIL_QUERY_H
#include <string>
#include "common.h"
#include "join.h"
#include "condition.h"
#include "order_by_.h"
#include "table.h"
#include "query.h"
#include "union.h"

namespace obelisk::database::builder::detail
{
    using selectable_t = alia<col, core::sql_value, raw>;
    using from_t = alia<table, raw>;

    class query: public base_builder_statement,public enable_where_condition<query>
    {
    public:

        std::string compile() override;
        query& join(join_ join);

        query& join(join_t join_table, std::vector<condition> conditions);
        query& left_join(join_t join_table, std::vector<condition> conditions){
            joins_.emplace_back(std::move(join_table), std::move(conditions), "LEFT");
            return *this;
        }
        query& right_join(join_t join_table, std::vector<condition> conditions){
            joins_.emplace_back(std::move(join_table), std::move(conditions), "RIGHT");
            return *this;
        }

        query& order_by(order_by_ order)
        {
            order_by_ = std::move(order);
            return *this;
        }
        query& from(std::initializer_list<from_t> tables)
        {
            std::ranges::copy(tables, std::back_inserter(from_));
            return *this;
        }

        query& select(std::initializer_list<selectable_t> selections)
        {
            std::ranges::copy(selections, std::back_insert_iterator(selects_));
            return *this;
        }

        query& u_nion(union_ target)
        {
            unions_.push_back(target);
            return *this;
        }

    private:
        bool distinct_ = false;
        std::vector<selectable_t> selects_;
        std::vector<from_t> from_;
        std::vector<join_> joins_;
        std::optional<order_by_> order_by_;
        std::vector<union_> unions_;
    };

}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_QUERY_H
