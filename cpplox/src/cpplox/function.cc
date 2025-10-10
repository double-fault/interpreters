#include "function.h"
#include "environment.h"
#include "interpreter.h"

namespace cpplox {

Function::Function(const std::string& name, std::vector<std::unique_ptr<Token>> parameters,
    const int arity, std::unique_ptr<IStatement> body)
    : mName { name }
    , mParameters { std::move(parameters) }
    , mArity { arity }
    , mBody { std::move(body) }
{
}

Object Function::Call(Interpreter* interpreter, std::vector<Object> arguments)
{
    std::unique_ptr<Environment> oldEnvironment = std::move(interpreter->mEnvironment);
    interpreter->mEnvironment = std::make_unique<Environment>(oldEnvironment.get());
    for (int i = 0; i < mArity; i++) {
        interpreter->mEnvironment->Define(mParameters[i]->mLexeme, arguments[i]);
    }

    try {
        mBody->Accept(interpreter);
    } catch (const Interpreter::ReturnException& exception) {
        ;
    }

    interpreter->mEnvironment = std::move(oldEnvironment);
    return interpreter->GetResult();
}

int Function::Arity()
{
    return mArity;
}

std::string Function::ToString()
{
    return mName;
}

}
