//
// Created by hackman on 5/13/25.
//

#ifndef BASE_SIGNER_H
#define BASE_SIGNER_H

#include <obelisk/http/core/raw.h>


namespace obelisk::http::client::signer {

class base_signer {
public:
    virtual ~base_signer() = default;
    virtual void sign_request(core::raw::http_request_raw& raw) = 0;
};

} // obelisk::http::client::signer

#endif //BASE_SIGNER_H
