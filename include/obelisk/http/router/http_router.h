/***********************************************************************************************************************
 * @author Hackman Lo
 * @file http_router.h
 * @description 
 * @created_at 2023-10-09
***********************************************************************************************************************/

#ifndef OBELISK_HTTP_ROUTER_H
#define OBELISK_HTTP_ROUTER_H
#include <memory>
#include <functional>
#include <string>

namespace obelisk::http {

    class http_response;
    class http_request;
    class route_item;
    class http_router {
    public:
        std::shared_ptr<http_response> handle(std::shared_ptr<http_request> request);
        route_item& add_route(const std::string& path, const std::function<std::shared_ptr<http_response> (std::shared_ptr<http_request>&)>& handler);
    protected:
        std::vector<route_item> routes_;
    };

} // obelisk

#endif //OBELISK_HTTP_ROUTER_H
