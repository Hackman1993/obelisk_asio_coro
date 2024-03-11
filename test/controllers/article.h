//
// Created by Hackman.Lo on 2024/3/1.
//

#ifndef ARTICLE_H
#define ARTICLE_H

#include "controller_base.h"
#include "http/core/http_request.h"

namespace obelisk::http {
    class http_response;
}

using namespace obelisk::http;
class article_controller: public controller_base {
public:
    static boost::cobalt::task<std::unique_ptr<http_response>> get_article_list(http_request_wrapper&request);
    static boost::cobalt::task<std::unique_ptr<http_response>> create_article(http_request_wrapper&request);
    static boost::cobalt::task<std::unique_ptr<http_response>> update_article(http_request_wrapper&request);
    static boost::cobalt::task<std::unique_ptr<http_response>> get_article_detail(http_request_wrapper&request);
    static boost::cobalt::task<std::unique_ptr<http_response>> delete_article(http_request_wrapper&request);
};




#endif //ARTICLE_H
