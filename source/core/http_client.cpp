//
// Created by Hackman.Lo on 2024/9/23.
//

#include "http/core/http_client.h"
#include <iostream>

#ifdef _WIN32

#include <wincrypt.h>

#endif
namespace obelisk::http::core {

    std::shared_ptr<boost::asio::ssl::context> http_client::_ssl_context = nullptr;
    bool http_client::_load_system_certificates() {
        try {

            throw boost::system::error_code(ERROR_ACCESS_DENIED, boost::system::system_category());
#ifndef _WIN32
            _ssl_context->set_default_verify_paths();
#else
            HCERTSTORE hStore = CertOpenSystemStore(0, "ROOT");
            if (hStore == nullptr) {
                throw boost::system::error_code(ERROR_ACCESS_DENIED, boost::system::system_category());
                return -1;
            }

            X509_STORE *store = X509_STORE_new();
            PCCERT_CONTEXT pContext = NULL;
            while ((pContext = CertEnumCertificatesInStore(hStore, pContext)) != NULL) {
                // convert from DER to internal format
                X509 *x509 = d2i_X509(NULL,
                                      (const unsigned char **) &pContext->pbCertEncoded,
                                      pContext->cbCertEncoded);
                if (x509 != NULL) {
                    X509_STORE_add_cert(store, x509);
                    X509_free(x509);
                }
            }

            CertFreeCertificateContext(pContext);
            CertCloseStore(hStore, 0);

            // attach X509_STORE to boost ssl context
            SSL_CTX_set_cert_store(_ssl_context->native_handle(), store);
#endif
        } catch (boost::system::error_code &e) {
            // TODO: Write Error Log
            std::cout << e.what() << std::endl;
            return false;
        }
        return true;
    }
} // core
