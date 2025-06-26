//
// Created by hackman on 5/19/25.
//

#ifndef GROUP_CONCAT_H
#define GROUP_CONCAT_H
#include "col.h"
#include "raw.h"

namespace obelisk::database::builder::detail
{

    using group_concat_t = std::variant<col, raw>;

    class group_concat : public base_statement{
    public:
        template<typename T>
        requires std::is_convertible_v<T, group_concat_t>
        group_concat(const T& column) : source_(column) {}

        template<typename T>
        requires std::is_same_v<T, col>||std::is_same_v<T, raw>
        group_concat(const T& column, const std::string& separator) : source_(column), separator_(separator) {}

        group_concat& distinct(bool enable = true)
        {
            distinct_ = enable;
            return *this;
        }

        std::string compile() override
        {
            std::string result = std::format("GROUP_CONCAT({}{}", distinct_? "DISTINCT ":"", std::visit(general_visitor{}, source_));
            if (separator_)
                result.append(std::format("SEPARATOR {}", separator_.value()));
            result.append(") ");
            return result;
        }
    private:
        group_concat_t source_;
        bool distinct_ = false;
        std::optional<std::string> separator_;
    };

}

#endif //GROUP_CONCAT_H
