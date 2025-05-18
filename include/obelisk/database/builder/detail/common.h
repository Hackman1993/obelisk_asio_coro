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
            const auto sql = compile();
            std::cout << sql << std::endl;
            auto result = co_await connection->template co_query<ResultType>(sql);
            co_return result;
        }

        template <typename ResultType>
        requires std::is_same_v<void, ResultType>
        boost::asio::awaitable<void> get()
        {
            auto connection = co_await db_pool::get_connection<mysql_connection>("default");
            const auto sql = compile();
            std::cout << sql << std::endl;
            co_await connection->co_query<ResultType>(sql);
            co_return;
        }

        template <typename ResultType, typename=std::enable_if_t<
            !std::is_same_v<boost::mysql::results, ResultType> &&
            boost::pfr::is_implicitly_reflectable_v<ResultType, struct t>
        >>
        boost::asio::awaitable<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>> get()
        {
            auto connection = co_await db_pool::get_connection<mysql_connection>("default");
            const auto sql = compile();
            std::cout << sql << std::endl;
            auto result = co_await connection->template co_query<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>>(sql);
            co_return result;
        }

        template <typename ResultType = boost::mysql::results, typename=std::enable_if_t<
            std::is_same_v<boost::mysql::results, ResultType>
        >>
        boost::asio::awaitable<ResultType> get(const std::shared_ptr<mysql_connection>& connection)
        {
            const auto sql = compile();
            std::cout << sql << std::endl;
            auto result = co_await connection->co_query<ResultType>(sql);
            co_return result;
        }

        template <typename ResultType, typename=std::enable_if_t<
            !std::is_same_v<boost::mysql::results, ResultType> &&
            boost::pfr::is_implicitly_reflectable_v<ResultType, t>
        >>
        boost::asio::awaitable<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>> get(const std::shared_ptr<mysql_connection>& connection)
        {
            const auto sql = compile();
            std::cout << sql << std::endl;
            auto result = co_await connection->co_query<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>>(sql);
            co_return result;
        }

        boost::asio::awaitable<std::uint64_t> count()
        {
            auto result = co_await get();
            co_return result.rows().size();
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

    protected:
        std::vector<condition_group> where_groups_;
    };
}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_COMMON_H
