//
// Created by hackman on 4/25/25.
//

#ifndef BUILDER_H
#define BUILDER_H
#include <boost/mysql/pfr.hpp>
#include <boost/mysql/results.hpp>
#include <obelisk/database/db_pool.h>
#include <obelisk/http/framework/config.h>

#include "columns.h"
#include "condition.h"
#include "condition_group.h"
#include "table.h"
#include "detail/utils.h"
#include <obelisk/database/mysql/mysql_connection.h>

#include "join.h"

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
            if (type_ == EST_INSERT)
            {
                std::string value_names;
                std::string value_str;
                for (int i =0; i< insert_columns_.size(); ++i)
                {
                    if (i > 0) value_names.append(",");
                    value_names.append(std::format("`{}`",insert_columns_[i]));
                }
                for (int i=0; i< insert_values_.size(); ++i)
                {
                    if (i > 0) value_str.append(",");
                    auto& values = insert_values_[i];
                    if (values.size() != insert_columns_.size())
                        throw std::logic_error("insert pack size not match");
                    std::string value_pack_str = "(";
                    for (int j=0; j< values.size(); ++j)
                    {
                        if (j > 0) value_pack_str.append(",");
                        value_pack_str.append(values[j].compile());
                    }
                    value_pack_str.append(")");
                    value_str.append(value_pack_str);
                }

                return std::format("INSERT INTO {}({}) VALUES {};" ,from_.empty()? "": from_[0].compile(), value_names, value_str);
            }
            if (type_ == EST_QUERY)
            {
                std::string join;
                for (int i = 0; i< joins_.size(); ++i)
                {
                    if (i != 0)
                        join.append("\n\t");
                    join.append(joins_[i].compile());
                }
                join.append(" ");
                result.append(std::format("SELECT {}{} FROM {} {}", distinct_? "DISTINCT ": "", utils::separate_with(select_columns_, ","), utils::separate_with(from_, ","), join));
            }
            if (type_ == EST_DELETE)
            {
                result.append(std::format("DELETE FROM {} ", utils::separate_with(from_, ",")));
            }
            if (type_ == EST_UPDATE)
            {
                std::string set_values;
                for (int i =0; i< set_pack_.size(); ++i)
                {
                    if (i != 0) set_values.append(",");
                    set_values.append(std::format("`{}` = {}", set_pack_[i].first, set_pack_[i].second.compile()));
                }
                result.append(std::format("UPDATE {} SET {} ", utils::separate_with(from_, ","), set_values));
            }
            if (!where_groups_.empty())
            {
                result.append(std::format("WHERE {}", utils::separate_with(where_groups_ ," OR ")));
            }
            result.append(";");
            return result;
        }
        explicit builder(const E_STATEMENT_TYPE type): type_(type)
        {
        }

        static builder update(const std::initializer_list<table>& tables)
        {
            builder result(EST_UPDATE);
            result.update_(tables);
            return result;
        }

        static builder insert(std::string table)
        {
            builder result(EST_INSERT);
            result.from_.emplace_back(std::move(table));
            return result;
        }

        static builder delete_from(std::initializer_list<table> tables)
        {
            builder result(EST_DELETE);
            result.delete_(tables);
            return result;
        }

        builder& set(const std::initializer_list<std::pair<std::string, sql_value>>& values)
        {
            if (type_ != EST_UPDATE)
                throw std::logic_error("server.error.cant_use_set_in_non_update");
            std::ranges::copy(values, std::back_inserter(set_pack_));
            return *this;
        }

        builder& inner_join(table join_table, std::vector<condition> conditions)
        {
            joins_.emplace_back(std::move(join_table), std::move(conditions), "INNER JOIN");
            return *this;
        }

        builder& values(const std::vector<std::pair<std::string, sql_value>>& value_pack)
        {
            insert_values_.clear();
            std::vector<std::string> insert_columns;
            std::vector<sql_value> insert_value;
            for (auto&[first, second]: value_pack)
            {
                insert_columns.emplace_back(first);
                insert_value.emplace_back(second);
            }
            insert_columns_ = std::move(insert_columns);
            insert_values_.emplace_back(std::move(insert_value));
            return *this;
        }

        builder& values(std::vector<std::string> columns, std::vector<std::vector<sql_value>> values)
        {
            insert_columns_ = std::move(columns);
            insert_values_ = std::move(values);
            return *this;
        }
        static builder select(std::initializer_list<col> args) {
            builder result(EST_QUERY);
            std::ranges::copy(args, std::back_inserter(result.select_columns_));
            return result;
        }

        builder& from(std::initializer_list<table> tables)
        {
            std::ranges::copy(tables, std::back_inserter(from_));
            return *this;
        }

        template <typename ResultType = boost::mysql::results, typename=std::enable_if_t<
            std::is_same_v<boost::mysql::results, ResultType>
        >>
        boost::asio::awaitable<ResultType> get()
        {
            auto tp = std::chrono::system_clock::now();
            auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("default");
            std::cout << compile() << std::endl;
            auto result = co_await connection->template co_query<ResultType>(compile());
            co_return result;
        }

        template <typename ResultType, typename=std::enable_if_t<
            !std::is_same_v<boost::mysql::results, ResultType> &&
            boost::pfr::is_implicitly_reflectable_v<ResultType, struct t>
        >>
        boost::asio::awaitable<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>> get()
        {
            auto tp = std::chrono::system_clock::now();
            auto connection = co_await obelisk::database::db_pool::get_connection<mysql_connection>("default");
            std::cout << compile() << std::endl;
            auto result = co_await connection->template co_query<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>>(compile());
            co_return result;
        }

        template <typename ResultType = boost::mysql::results, typename=std::enable_if_t<
            std::is_same_v<boost::mysql::results, ResultType>
        >>
        boost::asio::awaitable<ResultType> get(const std::shared_ptr<mysql_connection> connection)
        {
            auto tp = std::chrono::system_clock::now();
            std::cout << compile() << std::endl;
            auto result = co_await connection->template co_query<ResultType>(compile());
            co_return result;
        }

        template <typename ResultType, typename=std::enable_if_t<
            !std::is_same_v<boost::mysql::results, ResultType> &&
            boost::pfr::is_implicitly_reflectable_v<ResultType, struct t>
        >>
        boost::asio::awaitable<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>> get(const std::shared_ptr<mysql_connection> connection)
        {
            auto tp = std::chrono::system_clock::now();
            std::cout << compile() << std::endl;
            auto result = co_await connection->template co_query<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>>(compile());
            co_return result;
        }

        boost::asio::awaitable<std::uint64_t> count()
        {
            auto result = co_await get();
            co_return result.rows().size();
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

        builder& where(const std::initializer_list<condition>& conditions) {
            if (where_groups_.empty())
                where_groups_.emplace_back();

            auto &where = where_groups_.back();
            std::ranges::copy(conditions,std::back_inserter( where));
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

        builder& update_(const std::initializer_list<table>& tables)
        {
            std::ranges::move(tables, std::back_inserter(from_));
            return *this;
        }

        builder& delete_(const std::initializer_list<table>& tables)
        {
            std::ranges::move(tables, std::back_inserter(from_));
            return *this;
        }

        E_STATEMENT_TYPE type_;
        std::vector<table> from_;
        bool distinct_ = false;

        std::vector<join> joins_;
        std::vector<std::string> insert_columns_;
        std::vector<std::vector<sql_value>> insert_values_;
        std::vector<std::pair<std::string, sql_value>> set_pack_;
        std::vector<condition_group> where_groups_;
        std::vector<col> select_columns_;
    };
} // obelisk::database::query
#endif //BUILDER_H
