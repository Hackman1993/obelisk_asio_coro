//
// Created by hackman on 4/24/25.
//

#ifndef TABLE_BLUEPRINT_H
#define TABLE_BLUEPRINT_H
#include <memory>
#include "blueprint.h"
#include "column_blueprint.h"
#include <iostream>
namespace obelisk::database::migration
{
    class table_blueprint
    {
    public:
        table_blueprint(const std::string& table, const bool is_create,
                        const std::string& prefix = ""): creation_(is_create), table_(prefix + table)
        {
        }

        column_blueprint& primary(const std::string& column)
        {

            const auto ref = columns_.emplace_back(std::make_shared<column_blueprint>(table_, column, "bigint"));
            return ref->set_unsigned().auto_increment().primary();
        }



        column_blueprint& id()
        {
            return primary("id");
        }

        virtual column_blueprint& foreign_id(const std::string& column)
        {
            return integer(column, "bigint", true);
        }

        column_blueprint& integer(const std::string& column, const std::string& type = "int",
                                  const bool is_unsigned = false, const bool auto_increment = false)
        {
            const auto ref = columns_.emplace_back(std::make_shared<column_blueprint>(table_, column, type));
            ref->set_unsigned(is_unsigned);
            if (auto_increment)
                ref->auto_increment();
            return *ref;
        }

        column_blueprint& bigint(const std::string& column, const std::string& type = "bigint",
                                 const bool is_unsigned = false, const bool auto_increment = false)
        {
            return integer(column, type, is_unsigned, auto_increment);
        }

        column_blueprint& string(const std::string& column, unsigned int length = 255)
        {
            return *columns_.emplace_back(
                std::make_shared<column_blueprint>(table_, column, std::format("varchar({})", length)));
        }

        column_blueprint& float_col(const std::string& column, unsigned short total = 8, unsigned short places = 2,
                                    bool is_unsigned = false)
        {
            auto ref = std::make_shared<column_blueprint>(table_, column, std::format("float({}, {})", total, places));
            ref->set_unsigned(is_unsigned);
            return *columns_.emplace_back(ref);
        }

        column_blueprint& timestamp(const std::string& column)
        {
            auto ref = std::make_shared<column_blueprint>(table_, column, "timestamp");
            return *columns_.emplace_back(ref);
        }

        void index(const std::vector<std::string>& columns, bool unique = false, const std::string& name = "")
        {
            std::string index_name = name;
            if (index_name.empty())
            {
                index_name = unique? "idx_":"uniq_";
                for (auto & column : columns)
                {
                    index_name.append("_" + column);
                }
            }
            indexes_.emplace_back(index_name, unique, columns);
        }

        void timestamps()
        {
            timestamp("created_at").use_current();
            timestamp("updated_at").use_current().use_current_on_update();
        }

        void soft_delete()
        {
            timestamp("deleted_at").nullable();
        }

        std::vector<detail::basic_action> describe()
        {
            std::vector<detail::basic_action> commands;
            std::string column_describe;

            for (const auto& column : columns_)
            {
                if (!column_describe.empty()) column_describe.append(",\n\t");
                else column_describe.append("\n\t");
                if (creation_)
                    column_describe += column->single_describe();
                column->describe(commands, creation_);
            }

            if (creation_)
            {
                commands.insert(commands.begin(),
                                detail::basic_action(std::format("CREATE TABLE {} ({}\n);", table_, column_describe)));
            }
            if (!indexes_.empty())
            {
                for (auto& index_pair:indexes_)
                {
                    std::string columns_str;
                    auto &columns = std::get<std::vector<std::string>>(index_pair);
                    for (int i = 0; i < columns.size(); ++i)
                    {
                        if (i != 0) columns_str.append(", ");
                        columns_str.append(columns[i]);
                    }
                    commands.emplace_back(std::format("ALTER TABLE {} ADD {}INDEX {}({});", table_, std::get<bool>(index_pair)?"UNIQUE ":"" , std::get<std::string>(index_pair),columns_str));
                }

            }
            return commands;
        }

    private:
        bool creation_ = false;
        std::string table_;
        std::vector<std::tuple<std::string, bool, std::vector<std::string>>> indexes_;
        std::vector<std::shared_ptr<column_blueprint>> columns_;
    };
}
#endif //TABLE_BLUEPRINT_H
