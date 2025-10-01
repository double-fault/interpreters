#include "environment.h"
#include "parser.h"

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>
#include <memory>

namespace cpplox {

Environment::Environment()
    : mEnclosingEnvironment { nullptr }
{
}

Environment::Environment(std::unique_ptr<Environment> enclosingEnvironment)
    : mEnclosingEnvironment { std::move(enclosingEnvironment) }
{
}

void Environment::Define(const std::string& name, const Object& object)
{
    mVariables[name] = object;
}

void Environment::Assign(const std::string& name, const Object& object)
{
    if (mVariables.find(name) == mVariables.end()) {
        if (mEnclosingEnvironment != nullptr) {
            mEnclosingEnvironment->Assign(name, object);
            return;
        }
        throw ParserException(fmt::format("Invalid assignment, variable '{}' has not been defined", name));
    } else {
        Define(name, object);
    }
}

Object* Environment::Get(const std::string& name)
{
    if (mVariables.find(name) == mVariables.end()) {
        if (mEnclosingEnvironment != nullptr) {
            return mEnclosingEnvironment->Get(name);
        }
        throw ParserException(fmt::format("Variable '{}' not found in environment", name));
    }

    return &mVariables[name];
}

}
