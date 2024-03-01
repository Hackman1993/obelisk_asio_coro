//
// Created by Hackman.Lo on 2024/3/1.
//

#ifndef ARTICLE_CATEGORY_H
#define ARTICLE_CATEGORY_H

#include "http/core/http_request.h"

namespace obelisk::http {
    class http_response;
}

boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> create_article(obelisk::http::http_request_wrapper&request);




#endif //ARTICLE_CATEGORY_H
