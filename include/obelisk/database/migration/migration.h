//
// Created by hackman on 4/23/25.
//

#ifndef MIGRATION_H
#define MIGRATION_H

#include <functional>
#include <string>

#include "blueprint.h"
#define DEFINE_OBELISK_MIGRATION public: std::string migration_name() override { return __FILE__; }


namespace obelisk::database::migration
{
    class migration_base
    {
    public:
        virtual ~migration_base() = default;
        virtual std::string migration_name() = 0;

        virtual void up() = 0;
        virtual void down()=0;
    };


    class migration {
    public:
        static void create(const std::string& table, std::function<void (table_blueprint&)> call)
        {
            table_blueprint blueprint(table, true);
            call(blueprint);
        }

        static void drop_if_exists(const std::string&& table)
        {

        }
    };
    class create_migration_table : public migration_base
    {
        public: std::string migration_name() override
        {
            return __FILE__;
        }
        void up() override
        {
            migration::create("migrations", [](table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("migration", 50).unique().comment("迁移名称");
                blueprint.integer("batch").comment("批次");
                blueprint.timestamps();
            });
        }
        void down() override
        {
          migration::drop_if_exists("migrations");
        }
    };

}





#endif //MIGRATION_H
