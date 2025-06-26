#ifndef OBELISK_HTTP_CLIENT_REQUEST_H
#define OBELISK_HTTP_CLIENT_REQUEST_H
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <obelisk/http/core/raw.h>
#include <sahara/exception/exception_base.h>

namespace obelisk::http::client
{
    class request {
    public:
        virtual ~request() = default;

        request(const std::string& uri, const std::string& method)
        {
            boost::regex url_regex(R"(^(?<protocol>http|https):\/\/(?<domain>[a-zA-Z0-9.-]+)(?::(?<port>\d+))?(?<path>/[^?]*)?(?:\?(?<query>[^#]*))?$)", boost::regex::icase | boost::regex::no_char_classes);

            boost::smatch match;
            if (!boost::regex_match(uri, match, url_regex))
                THROW(sahara::exception::exception_base, "Invalid URL!", "HttpClient");
            protocol_ = match["protocol"].str();
            boost::algorithm::to_lower(protocol_);
            host_ = match["domain"].str();
            if (match["port"].matched)
                port_ = boost::lexical_cast<std::uint16_t>(match["port"].str());
            else
                port_ = protocol_ == "http"? 80:443;

            header_.meta_.p1_ = method;
            header_.meta_.p2_ = match["path"].matched? match["path"].str():"/";
            header_.meta_.p3_ = "HTTP/1.1";

            query_ = match["query"].matched? match["query"].str():"";
            header_.headers_["Host"] = std::format("{}{}", host_,match["port"].matched? ":" + match["port"].str(): "");
        }

    protected:
        virtual void before_send()
        {

        }
        http_header header_;
        std::string host_;
        std::uint16_t port_ = 80;
        std::string protocol_;
        std::string query_;

        std::shared_ptr<std::iostream> raw_body_;
    };

}

#endif //OBELISK_HTTP_CLIENT_REQUEST_H
