#include "function.h"
#include "environment.h"
#include "interpreter.h"

namespace cpplox {

Function::Function(const std::string& name, std::shared_ptr<Environment> closure,
    std::vector<std::unique_ptr<Token>> parameters, const int arity,
    std::vector<std::unique_ptr<IStatement>> body)
    : mName { name }
    , mClosure { closure }
    , mParameters { std::move(parameters) }
    , mArity { arity }
    , mBody { std::move(body) }
{
}

Object Function::Call(Interpreter* interpreter, std::vector<Object> arguments)
{
    std::shared_ptr<Environment> oldEnvironment = interpreter->mEnvironment;
    interpreter->mEnvironment = std::make_shared<Environment>(mClosure.get());

    for (int i = 0; i < mArity; i++) {
        interpreter->mEnvironment->Define(mParameters[i]->mLexeme, arguments[i]);
    }

    try {
        for (auto& statement : mBody) {
            statement->Accept(interpreter);
        }
    } catch (const Interpreter::ReturnException& exception) {
        ;
    }

    interpreter->mEnvironment = oldEnvironment;
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
