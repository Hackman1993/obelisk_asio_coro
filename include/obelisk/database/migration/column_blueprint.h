//
// Created by hackman on 4/24/25.
//

#ifndef COLUMN_BLUEPRINT_H
#define COLUMN_BLUEPRINT_H
#include <utility>
#include <vector>
#include <format>
#include <optional>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <obelisk/http/framework/config.h>

#include "basic_action.h"
#include "obelisk/database/builder/base_statement.h"

namespace obelisk::database::migration
{
    class column_blueprint
    {
    public:
        virtual ~column_blueprint() = default;

        column_blueprint(const std::string& table_name, std::string column, std::string type, const bool nullable = false) :
            column_(std::move(column)), type_(std::move(type)), table_(table_name), nullable_(nullable)
        {
        }

        virtual column_blueprint& after(const std::string& column)
        {
            after_ = " after " + column;
            return *this;
        }

        virtual column_blueprint& first(const std::string& column)
        {
            after_ = "first";
            return *this;
        }

        virtual column_blueprint& auto_increment(int start = 1)
        {
            auto_increment_ = start;
            return *this;
        }

        virtual column_blueprint& change()
        {
            update_ = true;
            return *this;
        }

        virtual column_blueprint& comment(const std::string& comment)
        {
            comment_ = comment;
            return *this;
        }

        virtual column_blueprint& default_value(sql_value value)
        {
            default_value_ = value.compile();
            return *this;
        }

        column_blueprint& index(const std::string& name = "", const bool is_unique = false)
        {
            const std::string prefix = is_unique? "unique_": "idx_";
            index_name_ = name.empty()? prefix + column_: name;
            index_unique_ = is_unique;
            return *this;
        }

        column_blueprint& unique(const std::string& name = "")
        {
            return index(name, true);
        }

        virtual column_blueprint& nullable(const bool value = true)
        {
            nullable_ = value;
            return *this;
        }

        virtual column_blueprint& primary()
        {
            is_primary_ = true;
            return *this;
        }


        virtual column_blueprint& set_unsigned(const bool value = true)
        {
            unsigned_ = value;
            return *this;
        }

        virtual column_blueprint& use_current()
        {
            default_value_ = "CURRENT_TIMESTAMP";
            return *this;
        }

        virtual column_blueprint& use_current_on_update()
        {
            use_current_on_update_ = true;
            return *this;
        }

        virtual void references(const std::string& reference_table , const std::string& reference_column)
        {
            reference_ = {reference_table, reference_column};
        }

        [[nodiscard]] bool is_update() const
        {
            return update_;
        }

        [[nodiscard]] bool need_rename() const
        {
            return new_column_name_.has_value();
        }

        std::string single_describe()
        {
            std::string column_describe = std::format("`{}` {}", new_column_name_? new_column_name_.value(): column_, type_);
            if (unsigned_)
                column_describe += " UNSIGNED";
            if (!nullable_)
                column_describe += " NOT NULL";
            if (is_primary_)
                column_describe += " PRIMARY KEY";
            if (auto_increment_)
                column_describe += " AUTO_INCREMENT";
            if (default_value_)
                column_describe += std::format(" DEFAULT ({})",default_value_.value());
            if (use_current_on_update_)
                column_describe += " ON UPDATE CURRENT_TIMESTAMP";
            if (after_){
                column_describe += after_.value();
            }
            if (comment_)
                column_describe += std::format(" COMMENT '{}'", comment_.value());
            return column_describe;
        }

        void describe(std::vector<detail::basic_action>& commands, const bool create_table = false)
        {
            if (!create_table)
            {
                if (!update_)
                    commands.emplace_back(std::format("ALTER TABLE {} ADD {};", table_, single_describe()));
                else
                    commands.emplace_back(std::format("ALTER TABLE {} {} {}{};", table_, new_column_name_? "CHANGE":"MODIFY", new_column_name_? column_ + " ":"", single_describe()));
            }


            if (auto_increment_){
                if (auto_increment_.value() != 1 && !update_)
                    commands.emplace_back(std::format("ALTER TABLE {} AUTO_INCREMENT={}", table_,auto_increment_.value()));
            }
            if (default_value_)
            {
                if (update_)
                    commands.emplace_back(std::format("ALTER TABLE {} ALTER COLUMN {} set default ({});", table_, column_, default_value_.value()));
            }

            if (index_name_)
            {
                if (update_)
                {
                    commands.emplace_back(std::format("ALTER TABLE {} DROP INDEX {};", table_, index_name_.value()),false);
                }
                commands.emplace_back(std::format("ALTER TABLE {} ADD {}INDEX {}({});", table_, index_unique_.value()?"UNIQUE ":"",index_name_.value(), column_));
            }

            if (reference_)
            {
                auto prefix = http::config::get<std::string>("database.default.prefix", "");
                const std::string foreign_key_name = std::format("fk_{}_{}_{}", column_, reference_.value().first, reference_.value().second);
                if (update_)
                {
                    commands.emplace_back(std::format("ALTER TABLE {} DROP FOREIGN KEY {};", table_, foreign_key_name), false);
                }

                commands.emplace_back(std::format("ALTER TABLE {} ADD CONSTRAINT {} FOREIGN KEY ({}) REFERENCES `{}{}`(`{}`);", table_,foreign_key_name, column_, prefix, reference_.value().first, reference_.value().second));

            }
        }

    protected:
        std::string column_;
        std::optional<std::string> after_;
        std::string type_;
        const std::string& table_;
        bool update_ : 1 = false;
        bool unsigned_ : 1 = false;
        bool nullable_ : 1 = false;
        bool is_primary_ : 1 = false;
        bool table_creation_ : 1 = false;
        bool use_current_on_update_ : 1 = false;
        std::optional<std::pair<std::string, std::string>> reference_;
        std::optional<int> auto_increment_;
        std::optional<bool> index_unique_;
        std::optional<std::string> index_name_;
        std::optional<std::string> comment_;
        std::optional<std::string> default_value_;
        std::optional<std::string> new_column_name_;
    };
}
#endif //COLUMN_BLUEPRINT_H
