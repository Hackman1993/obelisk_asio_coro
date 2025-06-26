//
// Created by hackman on 4/25/25.
//

#ifndef JOIN_STATEMENT_H
#define JOIN_STATEMENT_H
#include <utility>
#include "condition.h"
#include "table.h"
#include "utils.h"
#include "alia.h"

namespace obelisk::database::builder::detail
{
  using join_t = alia<table,raw, sub_query>;
  class join_ : public base_statement
  {
  public:
    join_(join_t table, std::vector<condition> conditions, std::string join_type = "INNER") : table_(std::move(table)), join_type_(std::move(join_type)), conditions_(std::move(conditions)) {}
    std::string compile() override
    {
      return std::format("{} JOIN {} ON {}", join_type_, table_.compile(), utils::separate_with(conditions_, " AND "));
    }

  private:
    join_t table_;
    std::string join_type_ = "INNER";
    std::vector<condition> conditions_;
  };
}
#endif //JOIN_STATEMENT_H
