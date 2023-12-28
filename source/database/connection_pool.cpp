//
// Created by Hackman.Lo on 2023/12/26.
//

#include "database/connection_pool.h"

namespace obelisk::database {
    std::shared_ptr<connection_manager> connection_manager::self_ptr_ = nullptr;
} // obelisk