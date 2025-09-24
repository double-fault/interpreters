#include "object.h"

#include <magic_enum/magic_enum.hpp>
#include <ostream>

namespace cpplox {

std::string Object::ToString() const
{
    std::string ret { magic_enum::enum_name(mType) };
    ret += "=";

    if (mType == Object::Type::kIdentifier) {
        ret += std::get<std::string>(mData);
    } else if (mType == Object::Type::kString) {
        ret += std::get<std::string>(mData);
    } else if (mType == Object::Type::kNumber) {
        ret += std::to_string(std::get<double>(mData));
    } else if (mType == Object::Type::kBool) {
        ret += std::to_string(std::get<bool>(mData));
    } else if (mType == Object::Type::kNil) {
        ret += "nil";
    }

    return ret;
}

std::ostream& operator<<(std::ostream& out, const Object& literal)
{
    out << literal.ToString();

    return out;
}

}
