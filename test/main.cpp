#include <boost/asio.hpp>
#include <boost/parser/parser.hpp>
#include <memory>
#include <middleware/cors.h>
#include <sahara/log/log.h>
#include <obelisk/http/framework.h>

#include "clients/filesystem/aliyun_oss_client.h"
#include "clients/filesystem/local_fs.h"
#include "module/default/default_.h"
#include "module/content/content.h"
#include "module/crown_plaza/crown_plaza.h"
#include <boost/parser/parser.hpp>
#include <boost/mysql/connection_pool.hpp>
#include "obelisk/database/mysql/mysql_connection_new.h"
using namespace boost::parser;
int main(int argc, char* argv[]) {
    try {
        boost::asio::io_context io_context{static_cast<int>(std::thread::hardware_concurrency()+1)};
        obelisk::database::experimental::any_pool_params<mysql_construct_params, boost::mysql::connect_params> params;
        params.connection_params.database = "obelisk1";
        params.connection_params.password = "hl97005497--";
        params.connection_params.server_address.emplace_host_and_port("127.0.0.1", 3306);
        params.connection_params.username = "root";
        obelisk::database::experimental::connection_pool<mysql_connection_new, mysql_construct_params,boost::mysql::connect_params> pool(io_context, params);
        pool.initialize();
        obelisk::http::framework fw(io_context);
        fw.register_fs("aliyunoss", [](const auto &config)
        {
            auto ak_id = config["access_key_id"].template get<std::string>();
            auto ak_secret = config["access_key_secret"].template get<std::string>();
            auto region = config["region"].template get<std::string>();
            auto bucket = config["bucket"].template get<std::string>();
            return std::make_shared<aliyun_oss_client>(ak_id, ak_secret,region, bucket);
        });
        fw.register_fs("filesystem", [](const auto &config)
        {
            auto access_domain = config["access_domain"].template get<std::string>();
            auto base_dir = config["base_dir"].template get<std::string>();
            return std::make_shared<local_fs>(base_dir, access_domain);
        });
        fw.module({
            std::make_shared<module::default_module>(),
            std::make_shared<module::crown_plaza_module>()
        });
        fw.middleware({std::make_shared<middleware::cors>()});
        std::vector<std::shared_ptr<std::thread>> threads;

        for (int i = 0; i < std::thread::hardware_concurrency(); i++)
        {
            threads.emplace_back(std::make_shared<std::thread>([&]()
            {
                fw.run();
            }));

        }
        fw.run();
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
