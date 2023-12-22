//
// Created by Hackman.Lo on 2023/12/21.
//

#include "http/response/empty_response.h"

namespace obelisk::http {
    empty_response::empty_response(): http_response(EResponseCode::EST_OK) {
        header_.headers_.emplace("Connection","keep-alive");
        header_.headers_.emplace("Content-Length","0");
        header_.headers_.emplace("Content-Type","application/json");
    }
} // obelisk::http