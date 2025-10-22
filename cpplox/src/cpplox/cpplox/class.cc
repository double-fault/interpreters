#include "class.h"
#include "fmt/base.h"
#include "interpreter.h"
#include "object.h"

#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <variant>

namespace cpplox {

Klass::Klass(const std::string& name, std::shared_ptr<Environment> closure,
    std::map<std::string, Object> methods)
    : mName { name }
    , mEnvironmentCapture { closure }
    , mClosure { closure }
    , mMethods { std::move(methods) }
{
    for (auto& [_, method] : mMethods) {
        if (!std::holds_alternative<std::shared_ptr<ICallable>>(method.mData)) {
            throw InterpreterException(fmt::format(
                "Internal error - method object is not callable in class {}", name));
        }
    }
}

Object Klass::Call(Interpreter* interpreter, std::vector<Object> arguments)
{
    const int arity { Arity() };
    if (arguments.size() != arity) {
        throw InterpreterException(fmt::format("Expected {} arguments in constructor, got {}",
            arity, arguments.size()));
    }

    std::shared_ptr<Instance> instance { std::make_shared<Instance>(
        fmt::format("<instance {}>", mName),
        shared_from_this()) };

    std::optional<Object> init = FindMethod("init");
    if (init != std::nullopt) {
        std::shared_ptr<ICallable> initializer {
            std::get<std::shared_ptr<ICallable>>(
                std::get<std::shared_ptr<ICallable>>(init->mData)->Bind(instance).mData)
        };
        initializer->Call(interpreter, arguments);
    }

    return Object(instance);
}

int Klass::Arity() const
{
    int arity { 0 };

    std::optional<Object> init = FindMethod("init");
    if (init != std::nullopt) {
        arity = std::get<std::shared_ptr<ICallable>>(init->mData)->Arity();
    }
    return arity;
}

std::string Klass::ToString() const
{
    return mName;
}

void Klass::Capture()
{
    if (mEnvironmentCapture == nullptr) {
        mEnvironmentCapture = mClosure.lock();
    }
    for (auto& [_, method] : mMethods) {
        std::get<std::shared_ptr<ICallable>>(method.mData)->Capture();
    }
}

void Klass::Release()
{
    for (auto& [_, method] : mMethods) {
        std::get<std::shared_ptr<ICallable>>(method.mData)->Release();
    }

    if (mEnvironmentCapture != nullptr) {
        mEnvironmentCapture.reset();
    }
}

Object Klass::Bind(std::shared_ptr<Instance> instance)
{
    throw InterpreterException("Class cannot be bound to instance");
}

std::optional<Object> Klass::FindMethod(const std::string& name) const
{
    if (mMethods.find(name) == mMethods.end()) {
        return std::nullopt;
    }
    return mMethods.at(name);
}

}
