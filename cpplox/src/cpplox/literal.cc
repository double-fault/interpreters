#include "literal.h"

#include <magic_enum/magic_enum.hpp>
#include <ostream>

namespace cpplox {

std::string Literal::ToString() const
{
    std::string ret { magic_enum::enum_name(mType) };
    ret += "=";

    if (mType == Literal::Type::kIdentifier) {
        ret += mIdentifier;
    } else if (mType == Literal::Type::kString) {
        ret += mString;
    } else if (mType == Literal::Type::kNumber) {
        ret += mNumber;
    }

    return ret;
}

std::ostream& operator<<(std::ostream& out, const Literal& literal)
{
    out << literal.ToString();

    return out;
}

}
