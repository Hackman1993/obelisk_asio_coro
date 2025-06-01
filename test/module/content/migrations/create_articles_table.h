#ifndef CREATE_ARTICLES_TABLE_H
#define CREATE_ARTICLES_TABLE_H
#include <obelisk/database/migration/migration.h>
namespace module::content::migrations
{
    class create_articles_table final : public obelisk::database::migration::base_migration{
        DEFINE_OBELISK_MIGRATION;
        boost::asio::awaitable<void> up() override
        {
            co_await obelisk::database::migration::migration::create("articles", [](obelisk::database::migration::table_blueprint& blueprint)
            {
                blueprint.id();
                blueprint.string("title");
                blueprint.text("content");
                blueprint.text("abstract");
                blueprint.timestamp("published_at").nullable().use_current();
                blueprint.string("cover_img").nullable();
                blueprint.timestamp("authorized_at").nullable();
                blueprint.foreign_id("fn_authorizor_id").nullable().references("sys_admins","id");
                blueprint.foreign_id("fn_category_id").references("article_categories", "id");
                blueprint.foreign_id("fn_author_id").references("sys_admins","id");
            });
            co_return;
        };
        boost::asio::awaitable<void> down() override
        {
            co_return;
        }
    };
}
#endif //CREATE_ARTICLES_TABLE_H
