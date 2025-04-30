//
// Created by hackman on 4/25/25.
//

#ifndef JOIN_STATEMENT_H
#define JOIN_STATEMENT_H
#include "base_statement.h"
#include "condition.h"
#include "table.h"
#include "detail/utils.h"

class join : public base_statement
{
public:
  join(table table, std::vector<condition> conditions, std::string join_type) : table_(std::move(table)), join_type_(std::move(join_type)), conditions_(std::move(conditions)) {}
  std::string compile() override
  {
    return std::format("{} {} ON {}", join_type_, table_.compile(), utils::separate_with(conditions_, " AND "));
  };

private:
  table table_;
  std::string join_type_ = "INNER JOIN";
  std::vector<condition> conditions_;
};
#endif //JOIN_STATEMENT_H
