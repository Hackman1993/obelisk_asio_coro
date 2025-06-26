//
// Created by hackman on 4/25/25.
//

#ifndef COLUMNS_H
#define COLUMNS_H
#include <vector>

#include "base_statement.h"
#include "alia.h"

class columns: public base_statement{


public:
    template<typename... Args>
    columns(Args&&... args) {
        (columns_.push_back(std::forward<Args>(args)), ...);
    }

protected:
    std::vector<alia> columns_;
};
#endif //COLUMNS_H
