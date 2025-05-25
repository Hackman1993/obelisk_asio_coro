//
// Created by hackman on 5/18/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_QUERY_H
#define OBELISK_DATABASE_BUILDER_DETAIL_QUERY_H
#include <string>

#include "common.h"
#include "join.h"
#include "condition.h"
#include "count.h"
#include "group_by.h"
#include "group_concat.h"
#include "if_format.h"
#include "limit.h"
#include "order_by_.h"
#include "table.h"
#include "query.h"
#include "union.h"

namespace obelisk::database::builder::detail
{
    class query;
    using selectable_t = alia<col, sql_value, raw, count_t, group_concat, if_format>;
    using from_t = alia<table, raw, query>;

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

        query& distinct()
        {
            distinct_ = true;
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
            selects_.clear();
            std::ranges::copy(selections, std::back_insert_iterator(selects_));
            return *this;
        }

        query& u_nion(const union_& target)
        {
            unions_.push_back(target);
            return *this;
        }

        query& limit(std::uint64_t lm, std::uint64_t offset = 0)
        {
            limit_ ={lm, offset};
            return *this;
        }

        query& group_by(const col& cl)
        {
            group_by_.emplace_back(cl);
            return *this;
        }

        query& group_by(const std::initializer_list<col>& cl)
        {
            std::ranges::copy(cl, std::back_inserter(group_by_));
            return *this;
        }

        [[nodiscard]] query& as_sub_query()
        {
            sub_query_ = true;
            return *this;
        }

        boost::asio::awaitable<std::uint64_t> count()
        {
            query q;
            query cp = *this;
            q.select({count_t(1)}).from({{cp.as_sub_query(), "count"}});
            auto result = co_await q.get();
            co_return result.rows()[0][0].as_int64();
        }

    private:
        bool sub_query_ = false;
        bool distinct_ = false;
        std::vector<selectable_t> selects_;
        std::vector<from_t> from_;
        std::vector<join_> joins_;
        std::optional<order_by_> order_by_;
        std::vector<union_> unions_;
        std::optional<limit_t> limit_;
        std::vector<col> group_by_;
    };

}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_QUERY_H
