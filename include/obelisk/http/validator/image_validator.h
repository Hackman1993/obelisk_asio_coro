//
// Created by Hackman.Lo on 2024/2/23.
//

#ifndef IMAGE_VALIDATOR_H
#define IMAGE_VALIDATOR_H

#include "file_validator.h"

namespace obelisk::http::validator {
    class image_validator :public file_validator{
    public:
        image_validator(): file_validator({".jpg", ".png"}){}

    };
    inline auto image(){ return std::make_shared<image_validator>(); }
}



#endif //IMAGE_VALIDATOR_H
