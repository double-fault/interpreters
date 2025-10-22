#pragma once

#include "icallable.h"
#include "instance.h"

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

    std::variant<std::monostate, double, bool, std::string, std::shared_ptr<ICallable>, std::shared_ptr<Instance>> mData;
};

}
