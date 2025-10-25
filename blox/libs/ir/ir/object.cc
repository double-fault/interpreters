#include "object.h"

#include <fmt/format.h>
#include <ostream>

namespace ir {

std::ostream& operator<<(std::ostream& out, const Object& object)
{
    out << object.ToString();
    return out;
}

std::string ObjectString::ToString() const
{
    return fmt::format("<string= {}>", mString);
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
