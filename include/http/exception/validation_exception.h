//
// Created by Hackman.Lo on 3/29/2023.
//

#ifndef OBELISK_VALIDATION_EXCEPTION_H
#define OBELISK_VALIDATION_EXCEPTION_H
#include "http_exception.h"
namespace obelisk::http {

  class validation_exception : public http_exception{
  public:
    explicit validation_exception(const std::string& what, EResponseCode code = EST_UNPROCESSABLE_CONTENT) : http_exception(what, code){}
  };

} // obelisk

#endif //OBELISK_VALIDATION_EXCEPTION_H
