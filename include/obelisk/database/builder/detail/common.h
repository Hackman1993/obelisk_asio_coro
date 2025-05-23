//
// Created by hackman on 5/17/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_COMMON_H
#define OBELISK_DATABASE_BUILDER_DETAIL_COMMON_H
#include <obelisk/database/core/common.h>
#include "condition_group.h"
#include <boost/mysql.hpp>
#include <obelisk/database/db_pool.h>
#include <boost/mysql/pfr.hpp>
#include <obelisk/database/mysql/mysql_connection.h>
class mysql_connection;
namespace obelisk::database::builder::detail
{

    class base_builder_statement: public core::base_statement
    {
public:
        template <typename ResultType = boost::mysql::results, typename=std::enable_if_t<
            std::is_same_v<boost::mysql::results, ResultType>
        >>
        boost::asio::awaitable<ResultType> get()
        {
            auto connection = co_await db_pool::get_connection<mysql_connection>("default");
            auto result = co_await connection->template co_query<ResultType>(this->compile());
            co_return result;
        }

        template <typename ResultType>
        requires std::is_same_v<void, ResultType>
        boost::asio::awaitable<void> get()
        {
            auto connection = co_await db_pool::get_connection<mysql_connection>("default");
            co_await connection->co_query<ResultType>(this->compile());
            co_return;
        }

        template <typename ResultType, typename=std::enable_if_t<
            !std::is_same_v<boost::mysql::results, ResultType> &&
            boost::pfr::is_implicitly_reflectable_v<ResultType, struct t>
        >>
        boost::asio::awaitable<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>> get()
        {
            auto connection = co_await db_pool::get_connection<mysql_connection>("default");
            auto result = co_await connection->template co_query<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>>(this->compile());
            co_return result;
        }

        template <typename ResultType = boost::mysql::results>
        std::enable_if_t<std::is_same_v<boost::mysql::results, ResultType>, boost::asio::awaitable<ResultType>> get(const std::shared_ptr<mysql_connection>& connection)
        {
            auto result = co_await connection->co_query<ResultType>(this->compile());
            co_return result;
        }

        template <typename ResultType>
        std::enable_if_t<std::is_same_v<ResultType, void>, boost::asio::awaitable<void>> get(const std::shared_ptr<mysql_connection>& connection)
        {
            boost::mysql::results results;
            auto result = co_await connection->co_query(this->compile());
            co_return;
        }

        template <typename ResultType>
        requires (!std::is_same_v<boost::mysql::results, ResultType> && !std::is_same_v<void, ResultType> && boost::pfr::is_implicitly_reflectable_v<ResultType, t>)
        boost::asio::awaitable<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>> get(const std::shared_ptr<mysql_connection>& connection)
        {
            auto result = co_await connection->co_query<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>>(this->compile());
            co_return result;
        }
    };

    template <typename T>
    class enable_where_condition
    {
    public:
        T& where(const std::initializer_list<condition>& conditions) {
            if (where_groups_.empty())
                where_groups_.emplace_back();

            auto &where = where_groups_.back();
            std::ranges::copy(conditions,std::back_inserter( where));
            return static_cast<T&>(*this);
        }

        T& or_where(const std::initializer_list<condition>& conditions)
        {
            where_groups_.emplace_back();
            auto &where = where_groups_.back();
            std::ranges::copy(conditions,std::back_inserter( where));
            return static_cast<T&>(*this);
        }

        T& global_where(const std::initializer_list<condition>& conditions)
        {
            std::ranges::copy(conditions,std::back_inserter( global_where_));
            return static_cast<T&>(*this);
        }

        std::string compile_where()
        {
            std::string result;
            if (!where_groups_.empty() || !global_where_.empty())
                result.append("WHERE ");
            if (!where_groups_.empty()){
                if (global_where_.empty() || where_groups_.size() == 1)
                    result.append(std::format("{}", utils::separate_with(where_groups_, " OR ")));
                else
                    result.append(std::format("({}) ", utils::separate_with(where_groups_, " OR ")));
            }
            if (!global_where_.empty())
            {
                if (!where_groups_.empty())
                    result.append("AND ");
                result.append(std::format("({}) ", utils::separate_with(global_where_, " AND ")));
            }
            return result;
        }


    protected:
        std::vector<condition_group> where_groups_;
        std::vector<condition> global_where_;
    };
}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_COMMON_H
