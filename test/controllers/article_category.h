//
// Created by Hackman.Lo on 2024/3/18.
//

#ifndef ARTICLE_CATEGORY_H
#define ARTICLE_CATEGORY_H


#include "controller_base.h"

namespace obelisk::http {
    class http_response;
}

class article_category: public controller_base {
public:
    static boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> get_article_category_list(obelisk::http::http_request_wrapper&request);
    static boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> get_article_category_full(obelisk::http::http_request_wrapper&request);
    static boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> create_article_category(obelisk::http::http_request_wrapper&request);
    static boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> update_article_category(obelisk::http::http_request_wrapper&request);
    static boost::cobalt::task<std::unique_ptr<obelisk::http::http_response>> delete_article_category(obelisk::http::http_request_wrapper&request);
};



#endif //ARTICLE_CATEGORY_H
