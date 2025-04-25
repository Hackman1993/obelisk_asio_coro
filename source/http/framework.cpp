//
// Created by hackman on 4/25/25.
//
#include <obelisk/http/framework.h>
namespace obelisk::http
{
    framework* framework::instance_ = nullptr; // 静态实例指针
    std::once_flag framework::flag_;
}