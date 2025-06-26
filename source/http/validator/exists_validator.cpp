#include <obelisk/http/validator/exists_validator.h>

#include "obelisk/database/db.h"
#include "obelisk/http/core/http_request.h"
#include "obelisk/http/exception/http_exception.h"

namespace obelisk::http::validator
{

    boost::asio::awaitable<void> exists_validator::validate(const std::string& name, http_request_wrapper& request)
    {
        using namespace  database::builder::detail;
        if (request.params().contains(name))
        {
            auto query = database::db::select({
             {count_t(1)}
            }).from({table_});
            auto& value = request.params()[name];
            if (value.is_boolean())
                query.where({{col(column_), static_cast<std::uint64_t>(value.get<bool>())}});
            else if (value.is_string())
                query.where({{col(column_), value.get<std::string>()}});
            else if (value.is_null())
                query.where({{col(column_), nullptr}});
            else if (value.is_number_float())
                query.where({{col(column_), value.get<float>()}});
            else if (value.is_number_integer())
                query.where({{col(column_), value.get<std::int64_t>()}});
            else if (value.is_number_unsigned())
                query.where({{col(column_), value.get<std::uint64_t>()}});
            if (soft_delete_)
                query.where({{col("deleted_at"), nullptr}});
            auto result = co_await query.get();
            auto count = result.rows()[0][0].as_int64();
            if (count <=0)
            {
                throw http_exception(std::format("server.error.validator.{}_exists", name), EST_UNPROCESSABLE_CONTENT);
            }
        }
        co_return;
    }
}
