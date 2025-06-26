#ifndef OBELISK_JSON_RESPONSE_H
#define OBELISK_JSON_RESPONSE_H
#include "../core/http_response.h"
#include <boost/json.hpp>
#include <nlohmann/adl_serializer.hpp>


namespace obelisk::http {

    class json_response : public http_response{
    public:
        explicit json_response(const nlohmann::json& json, EResponseCode code = EST_OK);
    };

} // obelisk

#endif //OBELISK_JSON_RESPONSE_H
