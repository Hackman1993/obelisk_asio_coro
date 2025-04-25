//
// Created by hackman on 4/25/25.
//

#ifndef BASE_MODULE_H
#define BASE_MODULE_H
namespace obelisk::http::module
{
    class base_module
    {
    public:
        virtual ~base_module() = default;
        virtual void routes()
        virtual void run() = 0;
        virtual void stop() = 0;
    };
}

#endif //BASE_MODULE_H
