#pragma once

#include "icallable.h"

#include <memory>
#include <string>
#include <variant>

namespace cpplox {

struct Object {
    Object()
        : mData { std::monostate {} }
    {
    }

    template <typename T>
    Object(const T& data)
        : mData { data }
    {
    }

    bool IsNil() const;
    std::string ToString() const;

    friend std::ostream& operator<<(std::ostream& out, const Object& token);

    // I'd like to keep Object copyable, hence the callable is a shared_ptr instead of a unique_ptr
    std::variant<std::monostate, double, bool, std::string, std::shared_ptr<ICallable>> mData;
};

}
