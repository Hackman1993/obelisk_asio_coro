//
// Created by Hackman.Lo on 2024/9/23.
//

#include "http/core/http_client.h"
#include "http/core/http_response.h"
#include <iostream>
#include <sahara/log/log.h>

#include "../../../../../vcpkg/installed/x64-windows/include/boost/algorithm/string/case_conv.hpp"
#include "http/parser/http_parser_v2.h"
#ifdef _WIN32

#include <wincrypt.h>

#endif
namespace obelisk::http::core {

    std::shared_ptr<boost::asio::ssl::context> http_client::_ssl_context = nullptr;
    bool http_client::_load_system_certificates() {
        boost::system::error_code ec;
        return _load_system_certificates(ec);
    }

    bool http_client::_load_system_certificates(boost::system::error_code &error) {
        try {
#ifndef _WIN32
            _ssl_context->set_default_verify_paths();
#else
            HCERTSTORE hStore = CertOpenSystemStore(0, "ROOT");
            if (hStore == nullptr) {
                error = boost::system::error_code(static_cast<int>(GetLastError()), boost::system::system_category());
                return false;
            }

            X509_STORE *store = X509_STORE_new();
            PCCERT_CONTEXT pContext = nullptr;
            while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != nullptr) {
                X509 *x509 = d2i_X509(nullptr,(const unsigned char **) &pContext->pbCertEncoded,static_cast<long>(pContext->cbCertEncoded));
                if (x509 != nullptr) {
                    X509_STORE_add_cert(store, x509);
                    X509_free(x509);
                }
            }

            CertFreeCertificateContext(pContext);
            CertCloseStore(hStore, 0);

            SSL_CTX_set_cert_store(_ssl_context->native_handle(), store);
#endif
        } catch (boost::system::error_code &e) {
            LOG_MODULE_ERROR("Obelisk", "Invalid Protocol: {}", e.what());
            return false;
        }
        return true;
    }

    boost::asio::awaitable<std::shared_ptr<http_response>>http_client::send_request(const std::string &uri, const std::string& method, std::unordered_map<std::string, std::string> headers,std::shared_ptr<std::istream> body) {
        auto result = parser::parse_split_url(uri);
        if(!result)
            co_return nullptr;
        boost::algorithm::to_lower(result->protocol);

        if(result->protocol!= "http" && result->protocol != "https"){
            LOG_MODULE_ERROR("Obelisk", "Invalid Protocol: {}", result->protocol);
            co_return nullptr;
        }
        auto upcased_method = boost::algorithm::to_upper_copy(method);
        boost::asio::ip::tcp::resolver resolver(ioctx_);
        auto endpoints = co_await resolver.async_resolve(result->host, result->protocol, boost::asio::use_awaitable);

        if(result->protocol == "https") {
            boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
        }

        co_return nullptr;
    }
} // core
