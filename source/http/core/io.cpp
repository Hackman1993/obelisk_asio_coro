//
// Created by hackman on 5/7/25.
//

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <obelisk/http/core/io.h>
#include <obelisk/http/exception/protocol_exception.h>
#include <obelisk/http/parser/http_parser_v3.h>

namespace obelisk::http::core
{
    using namespace boost::asio;
}
