//
// Created by hackman on 5/17/25.
//

#ifndef OBELISK_DATABASE_BUILDER_DETAIL_COMMON_H
#define OBELISK_DATABASE_BUILDER_DETAIL_COMMON_H
#include <boost/mysql.hpp>
#include <obelisk/database/db_pool.h>
#include <boost/mysql/pfr.hpp>
#include <obelisk/database/mysql/mysql_connection.h>
#include "utils.h"
class mysql_connection;
namespace obelisk::database::builder::detail
{
    using sql_value_t = std::variant<std::nullptr_t, double, bool, std::int64_t, std::string, std::uint64_t, std::chrono::system_clock::time_point>;
    class base_statement
    {
    public:
        virtual ~base_statement() = default;
        virtual std::string compile() = 0;
    };

    struct general_visitor {
        template <typename T, typename = std::enable_if_t<std::is_base_of_v<base_statement, T>>>
        std::string operator()(T& c) const {
            return c.compile();
        }
        std::string operator()(std::string& c) const {
            return std::format("'{}'", c);
        }
        std::string operator()(sql_value_t& c) const {
            return std::visit(*this, c);
        }
        template <typename T, typename = std::enable_if_t<std::is_base_of_v<base_statement, T>>>
        std::string operator()(std::vector<T>& c) const {
            return std::format("({})", utils::separate_with(c,","));
        }
        std::string operator()(std::chrono::system_clock::time_point& c) const
        {
            return std::format("'{:%F %T}'", c);
        }
        template <typename T, typename=std::enable_if_t<!std::is_class_v<T>>>
        std::string operator()(T c) const {
            return std::format("{}", c);
        }
        std::string operator()(std::nullptr_t) const {
            return "NULL";
        }
    };


    class sql_value : public sql_value_t, base_statement
    {
    public:
        using sql_value_t::variant;
        std::string compile() override
        {
            return std::visit(general_visitor{}, *this);
        }
    };

    class base_builder_statement: public base_statement
    {
public:
        template <typename ResultType = boost::mysql::results, typename=std::enable_if_t<
            std::is_same_v<boost::mysql::results, ResultType>
        >>
        boost::asio::awaitable<ResultType> get(const std::string& inst = "default")
        {
            auto connection = co_await db_pool::get_connection<mysql_connection>(inst);
            if (connection->inuse_)
                LOG_CRITICAL("{}", this->compile());
            connection->inuse_ = true;
            auto result = co_await connection->template co_query<ResultType>(this->compile());
            connection->inuse_ = false;
            co_return result;
        }

        template <typename ResultType>
        requires std::is_same_v<void, ResultType>
        boost::asio::awaitable<void> get(const std::string& inst = "default")
        {
            auto connection = co_await db_pool::get_connection<mysql_connection>(inst);
            if (connection->inuse_)
                LOG_CRITICAL("Wired{}", "");
            connection->inuse_ = true;
            co_await connection->co_query<ResultType>(this->compile());
            connection->inuse_ = false;
            co_return;
        }

        template <typename ResultType, typename=std::enable_if_t<
            !std::is_same_v<boost::mysql::results, ResultType> &&
            boost::pfr::is_implicitly_reflectable_v<ResultType, struct t>
        >>
        boost::asio::awaitable<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>> get(const std::string& inst = "default")
        {
            auto connection = co_await db_pool::get_connection<mysql_connection>(inst);
            if (connection->inuse_)
                LOG_CRITICAL("Wired{}", "");
            connection->inuse_ = true;
            auto result = co_await connection->template co_query<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>>(this->compile());
            connection->inuse_ = false;
            co_return result;
        }

        template <typename ResultType = boost::mysql::results>
        std::enable_if_t<std::is_same_v<boost::mysql::results, ResultType>, boost::asio::awaitable<ResultType>> get(const std::shared_ptr<mysql_connection>& connection)
        {
            if (connection->inuse_)
                LOG_CRITICAL("Wired{}", "");
            connection->inuse_ = true;
            auto result = co_await connection->co_query<ResultType>(this->compile());
            connection->inuse_ = false;
            co_return result;
        }

        template <typename ResultType>
        std::enable_if_t<std::is_same_v<ResultType, void>, boost::asio::awaitable<void>> get(const std::shared_ptr<mysql_connection>& connection)
        {
            boost::mysql::results results;
            if (connection->inuse_)
                LOG_CRITICAL("Wired{}", "");

            auto result = co_await connection->co_query(this->compile());
            connection->inuse_ = false;
            co_return;
        }

        template <typename ResultType>
        requires (!std::is_same_v<boost::mysql::results, ResultType> && !std::is_same_v<void, ResultType> && boost::pfr::is_implicitly_reflectable_v<ResultType, t>)
        boost::asio::awaitable<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>> get(const std::shared_ptr<mysql_connection>& connection)
        {
            if (connection->inuse_)
                LOG_CRITICAL("Wired{}", "");
            connection->inuse_ = true;
            auto result = co_await connection->co_query<boost::mysql::static_results<boost::mysql::pfr_by_name<ResultType>>>(this->compile());
            connection->inuse_ = false;
            co_return result;
        }
    };


}

#endif //OBELISK_DATABASE_BUILDER_DETAIL_COMMON_H
