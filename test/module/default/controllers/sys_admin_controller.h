//
// Created by hackman on 5/15/25.
//

#ifndef SYS_ADMIN_CONTROLLER_H
#define SYS_ADMIN_CONTROLLER_H
#include <obelisk/database/db.h>
#include <obelisk/http/core/http_request.h>
#include <obelisk/http/core/http_response.h>

namespace module::default_::controllers
{
    class sys_admin_controller{
    public:
        static boost::asio::awaitable<std::unique_ptr<obelisk::http::http_response>> backend_view(obelisk::http::http_request_wrapper&request)
        {
            obelisk::database::db::select({"id", "username", "phone"}).from({"sys_admins", "sa"})
            .inner_join({"sys_mid_admin_role", "smar"}, {{col{"smar.fn_admin_id"}, col{"sa.id"}}, {col{"sa.deleted_at"}, nullptr}})
            .inner_join({"sys_roles", "sr"}, {"sr.id", "smar.fn_role_id"})
            .where({
                {col("sa.deleted_at"), nullptr}
            }).get<obelisk::http::http_response>();
            co_return nullptr;
        }
    };
}

#endif //SYS_ADMIN_CONTROLLER_H
