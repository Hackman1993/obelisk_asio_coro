//
// Created by hackman on 4/27/25.
//

#include "template_.h"

#include <obelisk/database/db.h>
#include <obelisk/http/http_server.h>

#include "controllers/auth_controller.h"
#include "migrations/create_sys_access_tokens_table.h"
#include "migrations/create_sys_admin_table.h"
#include "migrations/create_sys_organization_table.h"
#include "migrations/create_sys_roles_table.h"
#include "migrations/create_sys_verify_codes_table.h"
#include "migrations/create_sys_permissions_table.h"
#include "middleware/backend_auth.h"
#include "migrations/insert_permissions_data.h"
namespace module {
    void template_module::route(obelisk::http::http_server& server)
    {
    }

    boost::asio::awaitable<void> template_module::migrate(){

        co_return;
    }
} // module