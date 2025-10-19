#include "function.h"
#include "environment.h"
#include "interpreter.h"

#include <memory>
#include <vector>

namespace cpplox {

Function::Function(const std::string& name, std::shared_ptr<Environment> closure,
    const std::vector<std::string>& parameters, const int arity,
    const std::vector<IStatement*>& body)
    : mName { name }
    , mEnvironmentCapture { closure }
    , mClosure { closure }
    , mParameters { parameters }
    , mArity { arity }
    , mBody { body }
{
    assert(mEnvironmentCapture != nullptr);
}

Object Function::Call(Interpreter* interpreter, std::vector<Object> arguments)
{
    std::shared_ptr<Environment> oldEnvironment = interpreter->mEnvironment;
    interpreter->mEnvironment = std::make_shared<Environment>(mClosure.lock());

    for (int i = 0; i < mArity; i++) {
        interpreter->mEnvironment->Define(mParameters[i], arguments[i]);
    }

    try {
        for (auto& statement : mBody) {
            statement->Accept(interpreter);
        }
    } catch (const Interpreter::ReturnException& exception) {
        ;
    }

    interpreter->mEnvironment->Release();
    interpreter->mEnvironment = oldEnvironment;
    return interpreter->GetResult();
}

int Function::Arity() const
{
    return mArity;
}

std::string Function::ToString() const
{
    return mName;
}

void Function::Capture()
{
    if (mEnvironmentCapture == nullptr) {
        mEnvironmentCapture = mClosure.lock();
    }
}

void Function::Release()
{
    assert(mEnvironmentCapture != nullptr);
    if (shared_from_this().use_count() == 2) {
        mEnvironmentCapture.reset();
    }
}

Object Function::Bind(std::shared_ptr<Instance> instance)
{
    if (mClosure.expired()) {
        throw InterpreterException(fmt::format(
            "Internal error while binding function {} - closure has been free'd",
            mName));
    }

    std::shared_ptr<Environment> scope = std::make_shared<Environment>(mClosure.lock());
    scope->Define("this", instance);

    return Object(std::make_shared<Function>(
        mName,
        scope,
        mParameters,
        mArity,
        mBody));
}

}
