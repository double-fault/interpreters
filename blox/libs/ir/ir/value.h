#pragma once

#include <fmt/ostream.h>

namespace ir {

class Object;
class ObjectString;
class ObjectFunction;

// Value is copyable
struct Value {
public:
    enum class Type {
        kError = 0,
        kNil,
        kNumber,
        kBool,
        kString,
        kFunction
    };

    Value();
    Value(double number);
    Value(bool boolean);
    Value(Object*) = delete;
    Value(ObjectString* string);
    Value(ObjectFunction* function);

    Type mType;
    union {
        double number;
        bool boolean;
        Object* object;
    } mAs;

    std::string ToString() const;
    friend std::ostream& operator<<(std::ostream& out, const Value& value);
};

}

template <>
struct fmt::formatter<ir::Value> : ostream_formatter { };
