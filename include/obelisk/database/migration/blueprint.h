//
// Created by hackman on 4/23/25.
//

#ifndef BLUEPRINT_H
#define BLUEPRINT_H
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace obelisk::database::migration
{
    class column_blueprint
    {
    public:
        virtual ~column_blueprint() = default;

        column_blueprint(const std::string& column, const std::string& type, bool nullable = false): column_(column), type_(type), nullable_(nullable)
        {
        }

        virtual column_blueprint& after(const std::string& column)
        {
            after_ = column;
            return *this;
        }

        virtual column_blueprint& auto_increment()
        {
            auto_increment_ = true;
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
        virtual column_blueprint& default_value();

        virtual column_blueprint& from(int start_with)
        {
            start_with_ = start_with;
            return *this;
        }

        virtual column_blueprint& index(const std::string& name = "")
        {
            index_name_ = name.empty() ? "idx_" + column_ : name;
            return *this;
        }

        virtual column_blueprint& nullable(const bool value = false)
        {
            nullable_ = value;
            return *this;
        }

        virtual column_blueprint& primary()
        {
            is_primary_ = true;
            return *this;
        }

        virtual column_blueprint& unique(const std::string& index_name = "")
        {
            unique_name_ = index_name.empty() ? "unique_" + column_ : index_name_;
            return *this;
        }

        virtual column_blueprint& set_unsigned(bool value = true)
        {
            unsigned_ = value;
            return *this;
        }
        virtual column_blueprint& use_current()
        {
            default_value_ = 'CURRENT_TIMESTAMP';
            return *this;
        }
        virtual column_blueprint& use_current_on_update()
        {
            use_current_on_update_ = true;
            return *this;
        }

    protected:
        std::string column_;
        std::string after_;
        std::string type_;

        bool update_ : 1 = false;
        bool use_current_on_update_ : 1 = false;
        bool unsigned_ : 1 = false;
        bool auto_increment_ : 1 = false;
        bool nullable_ : 1 = false;
        bool is_primary_ : 1 = false;
        std::optional<std::string> default_value_;
        std::string comment_ = "";
        std::optional<int> start_with_;
        std::optional<std::string> index_name_;
        std::optional<std::string> unique_name_;
    };

    class foreign_id_column_blueprint : public column_blueprint
    {
    public:
        foreign_id_column_blueprint(const std::string& column, const std::string& type): column_blueprint(column, type){ }

        foreign_id_column_blueprint& after(const std::string& column) override
        {
            column_blueprint::after(column);
            return *this;
        }
        foreign_id_column_blueprint& auto_increment() override
        {
            column_blueprint::auto_increment();
            return *this;
        }
        foreign_id_column_blueprint& change() override
        {
            column_blueprint::change();
            return *this;
        }
        foreign_id_column_blueprint& comment(const std::string& comment) override
        {
            column_blueprint::comment(comment);
            return *this;
        }
        foreign_id_column_blueprint& default_value() override
        {
            column_blueprint::default_value();
            return *this;
        }
        foreign_id_column_blueprint& from(int start_with) override
        {
            start_with_ = start_with;
            return *this;
        }
        foreign_id_column_blueprint& index(const std::string& name) override
        {
            index_name_ = name;
            return *this;
        }
        foreign_id_column_blueprint& nullable(const bool value) override
        {
            column_blueprint::nullable(value);
            return *this;
        }
        foreign_id_column_blueprint& primary() override
        {
            column_blueprint::primary();
            return *this;
        }
        foreign_id_column_blueprint& unique(const std::string& index_name) override
        {
            unique_name_ = index_name;
            return *this;
        }
        foreign_id_column_blueprint& set_unsigned(bool value = true) override
        {
            column_blueprint::set_unsigned(value);
            return *this;
        }
        foreign_id_column_blueprint& use_current() override
        {
            column_blueprint::use_current();
            return *this;
        }
        foreign_id_column_blueprint& use_current_on_update() override
        {
            column_blueprint::use_current_on_update();
            return *this;
        }

        foreign_id_column_blueprint& on(const std::string& table)
        {
            on_ = table;
            return *this;
        }

        foreign_id_column_blueprint& reference(const std::string& column)
        {
            reference_ = "id";
            return *this;
        }

    protected:
        std::optional<std::string> reference_;
        std::optional<std::string> on_;
    };

    class table_blueprint
    {
    public:
        table_blueprint(const std::string& table, bool is_create): table_(table), creation_(is_create)
        {
        }

        foreign_id_column_blueprint& foreign_id(const std::string& column)
        {
            auto ptr = std::make_shared<foreign_id_column_blueprint>(column, "bigint");
            ptr->set_unsigned();
            columns_.push_back(ptr);
            return *ptr;
        }

        column_blueprint& primary(const std::string& column)
        {
            auto ref = columns_.emplace_back(std::make_shared<column_blueprint>(column, "bigint"));
            ref->set_unsigned().auto_increment().primary();
            return *this;
        }

        column_blueprint& id()
        {
            return primary("id");
        }

        column_blueprint& integer(const std::string& column, const std::string& type = "int", bool is_unsigned = false, bool auto_increment = false)
        {
            auto ref = columns_.emplace_back(std::make_shared<column_blueprint>(column, type));
            ref->set_unsigned(is_unsigned);
            if (auto_increment)
                ref->auto_increment();
            columns_.push_back(ref);
            return *ref;
        }

        column_blueprint& bigint(const std::string& column, const std::string& type = "bigint", bool is_unsigned = false, bool auto_increment = false)
        {
            return integer(column, type, is_unsigned, auto_increment);
        }

        column_blueprint& string(const std::string& column, unsigned int length = 255)
        {
            return *columns_.emplace_back(std::make_shared<column_blueprint>(column, std::format("varchar({})", length)));
        }

        column_blueprint& float_col(const std::string& column, unsigned short total = 8, unsigned short places=2, bool is_unsigned = false)
        {
            auto ref = std::make_shared<column_blueprint>(column, std::format("float({}, {})", total, places));
            ref->set_unsigned(is_unsigned);
            return *columns_.emplace_back(ref);
        }

        column_blueprint& timestamp(const std::string& column)
        {
            auto ref = std::make_shared<column_blueprint>(column, "timestamp");
            ref->use_current();
            return *columns_.emplace_back(ref);
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
    private:
        bool creation_ = false;
        std::string table_;
        std::vector<std::shared_ptr<column_blueprint>> columns_;
    };
}

#endif //BLUEPRINT_H
