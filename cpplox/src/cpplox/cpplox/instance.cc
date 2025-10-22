#include "instance.h"
#include "class.h"
#include "icallable.h"
#include "interpreter.h"
#include "object.h"

#include <fmt/format.h>
#include <optional>
#include <variant>

namespace cpplox {

Instance::Instance(const std::string& name, std::shared_ptr<Klass> klass)
    : mName { name }
    , mKlass { klass }
{
}

std::string Instance::ToString() const
{
    return mName;
}

Object Instance::Get(const std::string& name)
{
    if (mFields.find(name) != mFields.end()) {
        return mFields[name];
    }

    std::optional<Object> method = mKlass->FindMethod(name);
    if (method != std::nullopt) {
        return Bind(method.value());
    }

    throw InterpreterException(fmt::format("Unknown property {} on instance {}",
        name, mName));
}

void Instance::Set(const std::string& name, const Object& object)
{
    mFields[name] = object;
}

Object Instance::Bind(const Object& object)
{
    if (!std::holds_alternative<std::shared_ptr<ICallable>>(object.mData)) {
        throw InterpreterException("Internal error - cannot bind to non-callable object");
    }

    std::shared_ptr<ICallable> callee = std::get<std::shared_ptr<ICallable>>(object.mData);
    return callee->Bind(shared_from_this());
}

}
