#pragma once

#include <string>
#include <variant>

namespace cpplox {

struct Object {
    enum class Type {
        kError = 0,

        kIdentifier,
        kString,
        kNumber,
        kBool,
        kNil
    };

    Object()
        : mType { Type::kNil }
    {
    }

    Object(bool value)
        : mType { Type::kBool }
        , mData { value }
    {
    }

    Object(std::string value)
        : mType { Type::kString }
        , mData { value }
    {
    }

    Object(double value)
        : mType { Type::kNumber }
        , mData { value }
    {
    }

    std::string ToString() const;

    friend std::ostream& operator<<(std::ostream& out, const Object& token);

    Type mType;
    std::variant<double, bool, std::string> mData;
};

};
