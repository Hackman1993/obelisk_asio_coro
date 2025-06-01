#include <boost/asio.hpp>
#include <middleware/cors.h>
#include <sahara/log/log.h>
#include <obelisk/http/framework.h>
#include "module/default/default_.h"
#include "module/content/content.h"
#include "module/crown_plaza/crown_plaza.h"

int main(int argc, char* argv[]) {
    try {

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
