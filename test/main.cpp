#include <boost/asio.hpp>
#include <boost/parser/parser.hpp>
#include <middleware/cors.h>
#include <sahara/log/log.h>
#include <obelisk/http/framework.h>

#include "clients/filesystem/aliyun_oss_client.h"
#include "clients/filesystem/local_fs.h"
#include "module/default/default_.h"
#include "module/content/content.h"
#include "module/crown_plaza/crown_plaza.h"
#include <boost/parser/parser.hpp>
using namespace boost::parser;
int main(int argc, char* argv[]) {
    try {
        std::unordered_map<std::string, std::string> meta_data;
        obelisk::http::framework::register_fs("aliyunoss", [](const auto &config)
        {
            auto ak_id = config["access_key_id"].template get<std::string>();
            auto ak_secret = config["access_key_secret"].template get<std::string>();
            auto region = config["region"].template get<std::string>();
            auto bucket = config["bucket"].template get<std::string>();
            return std::make_shared<aliyun_oss_client>(ak_id, ak_secret,region, bucket);
        });
        obelisk::http::framework::register_fs("filesystem", [](const auto &config)
        {
            auto access_domain = config["access_domain"].template get<std::string>();
            auto base_dir = config["base_dir"].template get<std::string>();
            return std::make_shared<local_fs>(base_dir, access_domain);
        });
        obelisk::http::framework::init();
        obelisk::http::framework::module({
            std::make_unique<module::default_module>(),
            std::make_unique<module::crown_plaza_module>()
        });
        obelisk::http::framework::middleware({
            std::make_unique<middleware::cors>()
        });
        std::vector<std::shared_ptr<std::thread>> threads;

        obelisk::http::framework::run();
     }
    catch (boost::mysql::error_with_diagnostics & err)
    {
        std::cout << err.get_diagnostics().client_message() << std::endl;
        std::cout << err.get_diagnostics().server_message() << std::endl;
    }
    catch (std::exception&err) {
        std::cout << err.what() << std::endl;
    }

    return 0;
}
