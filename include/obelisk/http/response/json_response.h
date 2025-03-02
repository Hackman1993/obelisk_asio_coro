#ifndef OBELISK_JSON_RESPONSE_H
#define OBELISK_JSON_RESPONSE_H
#include "../core/http_response.h"
#include <boost/json.hpp>


namespace obelisk::http {

    class json_response : public http_response{
    public:
        json_response(const boost::json::object& object, EResponseCode code = EST_OK);
        json_response(const boost::json::array& object, EResponseCode  code = EST_OK);
    };

} // obelisk

#endif //OBELISK_JSON_RESPONSE_H
