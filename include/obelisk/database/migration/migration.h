//
// Created by hackman on 4/23/25.
//

#ifndef MIGRATION_H
#define MIGRATION_H

#include <functional>
#include <string>

#include "blueprint.h"
#define DEFINE_OBELISK_MIGRATION public: const std::string& migration_name() override { return __FILE__; }


namespace obelisk::database::migration
{
    class migration_base
    {
    public:
        virtual ~migration_base() = default;
        virtual const std::string& migration_name() = 0;

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
    };
    class create_migration_table : public migration_base
    {
        DEFINE_OBELISK_MIGRATION
        void up() override
        {
            migration::create("members", [](table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("username", 50).unique().comment("用户名");
                blueprint.string("nickname", 50).nullable().comment("昵称");
                blueprint.string("real_name", 50).nullable().comment("真实姓名");
                blueprint.string("password", 100).nullable().comment("密码");
                blueprint.string("phone", 20).nullable().comment("手机号");
                blueprint.string("avatar").nullable().comment("头像路径");
                blueprint.string("wechat_openid").nullable().index().comment("微信OpenID");
                blueprint.timestamps();
            });
        }
        void down() override
        {

        }
    };

}





#endif //MIGRATION_H
