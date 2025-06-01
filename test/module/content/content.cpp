//
// Created by hackman on 4/27/25.
//

#include "content.h"

#include <obelisk/database/db.h>
#include <obelisk/http/http_server.h>

#include "migrations/create_article_categories_table.h"
#include "migrations/create_articles_table.h"

namespace module {
    void content_module::route(obelisk::http::http_server& server)
    {
        using namespace content;

    }

    boost::asio::awaitable<void> content_module::migrate(){
        co_await obelisk::database::db::run_migration({
            std::make_shared<content::migrations::create_article_categories_table>(),
            std::make_shared<content::migrations::create_articles_table>(),
        });
        co_return;
    }
} // module