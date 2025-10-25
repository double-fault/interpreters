#include "environment.h"
#include "icallable.h"
#include "interpreter.h"
#include "parser.h"

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>
#include <variant>

namespace cpplox {

Environment::Environment()
    : mEnclosingEnvironment { nullptr }
    , mReleased { false }
{
}

Environment::Environment(std::shared_ptr<Environment> enclosingEnvironment)
    : mEnclosingEnvironment { enclosingEnvironment }
    , mReleased { false }
{
}

Environment::~Environment()
{
    assert(mReleased);
}

void Environment::Define(const std::string& name, const Object& object)
{
    mVariables[name] = object;
}

void Environment::Assign(const std::string& name, const Object& object)
{
    if (mVariables.find(name) == mVariables.end()) {
        throw ParserException(fmt::format("Invalid assignment, variable '{}' has not been defined", name));
    } else {
        mVariables[name] = object;
    }
}

Object Environment::Get(const std::string& name)
{
    if (mVariables.find(name) == mVariables.end()) {
        throw InterpreterException(fmt::format("Variable '{}' not found in environment", name));
    }

    Object& object { mVariables[name] };
    if (std::holds_alternative<std::shared_ptr<ICallable>>(object.mData)) {
        std::get<std::shared_ptr<ICallable>>(object.mData)->Capture();
    }

    return object;
}

void Environment::Release()
{
    assert(!mReleased);
    for (auto& [_, variable] : mVariables) {
        if (std::holds_alternative<std::shared_ptr<ICallable>>(variable.mData)) {
            std::get<std::shared_ptr<ICallable>>(variable.mData)->Release();
        }
    }
    mReleased = true;
}

}
