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

Value::Value(const std::string& string)
    : mType { Type::kString }
{
    Object* object = new ObjectString(string);
    mAs.object = object;
}

Value::Value(std::string_view string)
    : Value(std::string { string })
{
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
    // gcc gives a warning, but clangd does not
    return std::string {};
}

bool operator==(const Value& a, const Value& b)
{
    if (a.mType != b.mType) {
        return false;
    }

    switch (a.mType) {
    case Value::Type::kNil:
        return true;
    case Value::Type::kNumber:
        return a.mAs.number == b.mAs.number;
    case Value::Type::kBool:
        return a.mAs.boolean == b.mAs.boolean;
    case Value::Type::kString: {
        ObjectString* x = static_cast<ObjectString*>(a.mAs.object);
        ObjectString* y = static_cast<ObjectString*>(b.mAs.object);
        return x->mString == y->mString;
    }
    case Value::Type::kFunction:
        return a.mAs.object == b.mAs.object;
    default:
        assert(6 > 9);
    }
    return false; // unreachable
}

std::ostream& operator<<(std::ostream& out, const Value& value)
{
    out << value.ToString();
    return out;
}

}
