//
// Created by hackman on 4/25/25.
//

#ifndef BUILDER_H
#define BUILDER_H
#include <boost/mysql/results.hpp>
#include <obelisk/database/db_pool.h>
#include <obelisk/http/framework/config.h>

#include "columns.h"
#include "condition.h"
#include "condition_group.h"
#include "table.h"
#include "detail/utils.h"
#include <obelisk/database/mysql/mysql_connection.h>

namespace obelisk::database::query
{
    class builder : public base_statement
    {
    public:
        enum E_STATEMENT_TYPE
        {
            EST_QUERY,
            EST_UPDATE,
            EST_INSERT,
            EST_DELETE
        };
        std::string compile() override
        {
            std::string result;
            if (type_ == EST_QUERY)
            {
                result.append(std::format("SELECT {}{} FROM {}", distinct_? "DISTINCT ": "", utils::separate_with(select_columns_, ","), utils::separate_with(from_, ",")));
            }
            if (!where_groups_.empty())
            {
                result.append(std::format("WHERE {}", utils::separate_with(where_groups_ ," OR ")));
            }
            return result;
        }
        explicit builder(const E_STATEMENT_TYPE type): type_(type)
        {
        }
        static builder select(std::initializer_list<col> args) {
            builder result(EST_QUERY);
            std::ranges::copy(args, std::back_inserter(result.select_columns_));
            return result;
        }

        builder& from(std::initializer_list<table> tables)
        {
            std::copy(tables.begin(), tables.end(), std::back_inserter(from_));
            return *this;
        }

        boost::asio::awaitable<boost::mysql::results> get()
        {
            auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("default");
            boost::mysql::results results = co_await connection->co_query(compile());
            co_return results;
        }

        boost::asio::awaitable<void> execute()
        {
            auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("default");
            co_await connection->co_execute(compile());
            co_return;
        }

        // template<typename... Args>
        // builder& where(Args&&... args) {
        //     if (where_groups_.empty())
        //         where_groups_.emplace_back();
        //
        //     auto &where = where_groups_.back();
        //     (where.emplace_back(condition(std::forward<Args>(args))), ...);
        //     return *this;
        // }

        builder& where(std::initializer_list<condition> conditions) {
            if (where_groups_.empty())
                where_groups_.emplace_back();

            auto &where = where_groups_.back();
            std::copy(conditions.begin(), conditions.end(),std::back_inserter( where));
            return *this;
        }
        // builder& where(std::vector<condition>& conditions)
        // {
        //     if (where_groups_.empty())
        //         where_groups_.emplace_back();
        //     auto &where = where_groups_.back();
        //     for (auto &condition: conditions)
        //     {
        //         where.push_back(condition);
        //     }
        //     return *this;
        // }



    protected:
        builder()=default;
        template<typename... Args>
        builder& select_(Args&&... args) {
            type_ = EST_QUERY;
            (select_columns_.emplace_back(col(std::forward<Args>(args))), ...);
            return *this;
        }
        builder& where_(condition t)
        {
            if (where_groups_.empty())
                where_groups_.emplace_back();

            auto &where = where_groups_.back();
            where.push_back(std::move(t));
            return *this;
        }

        E_STATEMENT_TYPE type_;
        std::vector<table> from_;
        bool distinct_ = false;
        std::vector<condition_group> where_groups_;
        std::vector<col> select_columns_;
    };
} // obelisk::database::query
#endif //BUILDER_H
