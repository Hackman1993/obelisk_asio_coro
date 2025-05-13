//
// Created by hackman on 5/13/25.
//

#ifndef ALIYUN_SMS_CLIENT_H
#define ALIYUN_SMS_CLIENT_H
#include <obelisk/http/core/base_client.h>

#include "obelisk/http/client/signer/aliyun_openapi.h"


class aliyun_sms_client {
public:
    aliyun_sms_client(const std::string& ak_id, const std::string& ak_secret): http_client_(std::make_unique<obelisk::http::client::signer::aliyun_openapi>(ak_id, ak_secret))
    {
    }

    boost::asio::awaitable<bool> send_sms(const std::string& phone, const std::string& sign_name,const std::string& template_code,std::string template_param )
    {
        const std::string url = std::format("http://dysmsapi.aliyuncs.com/?PhoneNumbers={}&TemplateCode={}&TemplateParam={}&SignName={}", phone, template_code, template_param, sign_name);
        const auto resp = co_await http_client_.send_request(url, "POST", {
            {"x-acs-action", "SendSms"},
            {"x-acs-version", "2017-05-25"}
        }, nullptr);
        if (resp->header_raw().meta_.p2_.starts_with("20"))
            co_return true;
        throw std::runtime_error(resp->header_raw().meta_.p3_);
    }

private:
    obelisk::http::core::base_client http_client_;
};



#endif //ALIYUN_SMS_CLIENT_H
