#include "value.h"
#include "object.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace ir {

Value::Value()
    : mType { Type::kNil }
{
}

Value::Value(double number)
    : mType { Type::kNumber }
{
    mAs.number = number;
}

Value::Value(bool boolean)
    : mType { Type::kBool }
{
    mAs.boolean = boolean;
}

Value::Value(ObjectString* string)
    : mType { Type::kString }
{
    mAs.object = static_cast<Object*>(string);
}

Value::Value(ObjectFunction* function)
    : mType { Type::kFunction }
{
    mAs.object = static_cast<Object*>(function);
}

std::string Value::ToString() const
{
    switch (mType) {
    case Type::kNumber:
        return fmt::format("number= {}", mAs.number);
    case Type::kNil:
        return fmt::format("nil");
    case Type::kBool:
        return fmt::format("boolean= {}", mAs.boolean);
    case Type::kString:
        return fmt::format("string= {}", mAs.object->ToString());
    case Type::kFunction:
        return fmt::format("function= {}", mAs.object->ToString());
    case Type::kError:
        spdlog::error("Value type enum is kError!");
        exit(1);
    }
}

std::ostream& operator<<(std::ostream& out, const Value& value)
{
    out << value.ToString();
    return out;
}

}
