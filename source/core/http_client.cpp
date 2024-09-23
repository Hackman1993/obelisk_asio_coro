//
// Created by Hackman.Lo on 2024/9/23.
//

#include "http/core/http_client.h"
#include "http/core/http_response.h"
#include <iostream>

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
            // TODO: Write Error Log
            std::cout << e.what() << std::endl;
            return false;
        }
        return true;
    }

    boost::asio::awaitable<std::shared_ptr<http_response>>http_client::send_request(const std::string &uri, std::unordered_map<std::string, std::string> headers,std::shared_ptr<std::istream> body) {

        co_return nullptr;
    }
} // core
