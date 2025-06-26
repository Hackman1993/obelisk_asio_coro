//
// Created by hackman on 4/29/25.
//

#ifndef CREATE_ARTICLE_CATEGORY_TABLE_H
#define CREATE_ARTICLE_CATEGORY_TABLE_H
#include <obelisk/database/migration/migration.h>
namespace module::content::migrations
{
    class create_article_categories_table final : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await obelisk::database::migration::migration::create("article_categories", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("name");
                blueprint.string("description");
                blueprint.boolean("visible").default_value(1);
                blueprint.string("cover_img").nullable();
                blueprint.timestamps();
                blueprint.soft_delete();
            });
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //CREATE_ARTICLE_CATEGORY_TABLE_H
