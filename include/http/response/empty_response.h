//
// Created by Hackman.Lo on 2023/12/21.
//

#ifndef EMPTY_RESPONSE_H
#define EMPTY_RESPONSE_H
#include "../core/http_response.h"
namespace obelisk::http {

class empty_response:public http_response {
public:
    empty_response();
};

} // obelisk::http

#endif //EMPTY_RESPONSE_H
