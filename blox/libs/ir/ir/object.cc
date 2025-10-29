#include "object.h"

#include <fmt/format.h>
#include <ostream>

namespace ir {

std::ostream& operator<<(std::ostream& out, const Object& object)
{
    out << object.ToString();
    return out;
}

ObjectString::ObjectString(std::string_view string)
    : mString { string }
{
}

std::string ObjectString::ToString() const
{
    return mString;
}

ObjectFunction::ObjectFunction(const std::string& name, Type type, const int arity)
    : mName { name }
    , mType { type }
    , mArity { arity }
{
}

std::string ObjectFunction::ToString() const
{
    return fmt::format("<function= {}>", mName);
}

}
