#include "interpreter.h"
#include "ast.h"
#include "class.h"
#include "environment.h"
#include "function.h"
#include "object.h"
#include "token.h"

#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <variant>
#include <vector>

namespace cpplox {

Interpreter::Interpreter(const std::vector<IStatement*>& statements)
    : mStatements { statements }
    , mGlobals { std::make_shared<Environment>() }
    , mEnvironment { mGlobals }
{
}

void Interpreter::Run()
{
    for (auto& statement : mStatements) {
        if (statement != nullptr)
            statement->Accept(this);
    }
}

void Interpreter::Resolve(IExpression* expression, int depth)
{
    mResolvedLocals[expression] = depth;
}

Environment* Interpreter::ResolutionLookup(IExpression* expression)
{
    if (mResolvedLocals.find(expression) == mResolvedLocals.end()) {
        return mGlobals.get();
    }
    int depth = mResolvedLocals[expression];

    Environment* environment { mEnvironment.get() };
    while (depth--) {
        environment = environment->mEnclosingEnvironment.get();
    }
    return environment;
}

bool Interpreter::IsTrue(const Object& object)
{
    if (object.IsNil())
        return false;
    if (std::holds_alternative<bool>(object.mData))
        return std::get<bool>(object.mData);
    return true;
}

bool Interpreter::IsEqual(const Object& a, const Object& b)
{
    return a.mData == b.mData;
}

Object Interpreter::GetResult()
{
    return mResult;
}

Object Interpreter::Evaluate(IExpression* expression)
{
    expression->Accept(this);
    return GetResult();
}

void Interpreter::Visit(IStatement* statement)
{
    statement->Accept(this);
}

void Interpreter::Visit(StatementExpression* statement)
{
    mResult = Evaluate(statement->mExpression.get());
}

void Interpreter::Visit(StatementPrint* print)
{
    mResult = Evaluate(print->mExpression.get());
    std::cout << mResult << "\n";
}

void Interpreter::Visit(StatementVariable* variable)
{
    Object initializer {};
    if (variable->mInitializer != nullptr) {
        initializer = Evaluate(variable->mInitializer.get());
    }
    mEnvironment->Define(variable->mName, initializer);
}

void Interpreter::Visit(StatementBlock* block)
{
    std::shared_ptr<Environment> oldEnvironment = mEnvironment;
    mEnvironment = std::make_shared<Environment>(oldEnvironment);
    for (auto& statement : block->mStatements) {
        if (statement != nullptr)
            statement->Accept(this);
    }
    mEnvironment->Release();
    mEnvironment = oldEnvironment;
}

void Interpreter::Visit(StatementIf* ifStatement)
{
    if (IsTrue(Evaluate(ifStatement->mCondition.get()))) {
        if (ifStatement->mThenStatement != nullptr)
            ifStatement->mThenStatement->Accept(this);
    } else if (ifStatement->mElseStatement != nullptr) {
        ifStatement->mElseStatement->Accept(this);
    }
}

void Interpreter::Visit(StatementWhile* whileStatement)
{
    while (IsTrue(Evaluate(whileStatement->mCondition.get()))) {
        whileStatement->mBody->Accept(this);
    }
}

void Interpreter::Visit(StatementFunction* function)
{
    std::vector<IStatement*> body;
    for (auto& statement : function->mBody) {
        body.push_back(statement.get());
    }
    mResult = Object(std::make_shared<Function>(
        function->mIdentifier,
        mEnvironment,
        function->mParameters,
        function->mParameters.size(),
        body));
    mEnvironment->Define(function->mIdentifier, mResult);
}

void Interpreter::Visit(StatementReturn* returnStatement)
{
    if (returnStatement->mExpression != nullptr) {
        returnStatement->mExpression->Accept(this);
    }
    throw ReturnException {};
}

void Interpreter::Visit(StatementClass* klass)
{
    std::shared_ptr<Environment> oldEnvironment = mEnvironment;
    mEnvironment = std::make_shared<Environment>(oldEnvironment);

    std::map<std::string, Object> methods;
    for (auto& method : klass->mMethods) {
        method->Accept(this);

        if (!std::holds_alternative<std::shared_ptr<ICallable>>(mResult.mData)) {
            throw InterpreterException("Internal error - class method is not a method");
        }
        methods[std::get<std::shared_ptr<ICallable>>(mResult.mData)->ToString()] = mResult;
    }

    mEnvironment->Release();
    mEnvironment = oldEnvironment;

    mEnvironment->Define(klass->mIdentifier, std::make_shared<Klass>(klass->mIdentifier, mEnvironment, methods));
}

void Interpreter::Visit(IExpression* expression)
{
    expression->Accept(this);
}

void Interpreter::Visit(ExpressionObject* object)
{
    mResult = object->mObject;
}

void Interpreter::Visit(ExpressionGrouping* grouping)
{
    mResult = Evaluate(grouping->mExpression.get());
}

void Interpreter::Visit(ExpressionUnary* unary)
{
    mResult = Evaluate(unary->mExpression.get());

    switch (unary->mOperator) {
    case Token::Type::kMinus:
        AssertType<double>(mResult);
        mResult.mData = -std::get<double>(mResult.mData);
        break;

    case Token::Type::kBang:
        mResult = Object(IsTrue(mResult));
        break;

    default:
        throw InterpreterException("Interpreter internal error while interpreting type ExpressionUnary");
    }
}

void Interpreter::Visit(ExpressionLogical* logical)
{
    Object left = Evaluate(logical->mLeft.get());

    switch (logical->mOperator) {
    case Token::Type::kOr:
        if (IsTrue(left)) {
            mResult = left;
            break;
        }
        mResult = Evaluate(logical->mRight.get());
        break;

    case Token::Type::kAnd:
        if (!IsTrue(left)) {
            mResult = left;
            break;
        }
        mResult = Evaluate(logical->mRight.get());
        break;

    default:
        throw InterpreterException("Interpreter internal error while interpreting ExpressionLogical");
    }
}

void Interpreter::Visit(ExpressionBinary* binary)
{
    Object left = Evaluate(binary->mLeft.get());
    Object right = Evaluate(binary->mRight.get());

    switch (binary->mOperator) {
    case Token::Type::kMinus:
        AssertType<double>(left, right);
        mResult = Object(std::get<double>(left.mData) - std::get<double>(right.mData));
        break;

    case Token::Type::kSlash:
        AssertType<double>(left, right);
        if (std::get<double>(right.mData) == 0) {
            throw InterpreterException("Divide by zero");
        }

        mResult = Object(std::get<double>(left.mData) / std::get<double>(right.mData));
        break;

    case Token::Type::kStar:
        AssertType<double>(left, right);

        mResult = Object(std::get<double>(left.mData) * std::get<double>(right.mData));
        break;

    case Token::Type::kPlus:
        AssertType<double, std::string>(left);
        if (std::holds_alternative<double>(left.mData)) {
            AssertType<double>(right);
            mResult = Object(std::get<double>(left.mData) + std::get<double>(right.mData));
        } else {
            AssertType<std::string>(right);
            mResult = Object(std::get<std::string>(left.mData) + std::get<std::string>(right.mData));
        }
        break;

    case Token::Type::kGreater:
        AssertType<double>(left, right);
        mResult = Object(std::get<double>(left.mData) > std::get<double>(right.mData));
        break;

    case Token::Type::kGreaterEqual:
        AssertType<double>(left, right);
        mResult = Object(std::get<double>(left.mData) >= std::get<double>(right.mData));
        break;

    case Token::Type::kLess:
        AssertType<double>(left, right);
        mResult = Object(std::get<double>(left.mData) < std::get<double>(right.mData));
        break;

    case Token::Type::kLessEqual:
        AssertType<double>(left, right);
        mResult = Object(std::get<double>(left.mData) <= std::get<double>(right.mData));
        break;

    case Token::Type::kEqualEqual:
        mResult = Object(IsEqual(left, right));
        break;

    case Token::Type::kBangEqual:
        mResult = Object(!IsEqual(left, right));
        break;

    default:
        throw InterpreterException("Interpreter internal error while interpreting type ExpressionBinary");
    }
}

void Interpreter::Visit(ExpressionVariable* variable)
{
    mResult = ResolutionLookup(static_cast<IExpression*>(variable))
                  ->Get(variable->mName);
}

void Interpreter::Visit(ExpressionAssignment* assignment)
{
    mResult = Evaluate(assignment->mValue.get());
    ResolutionLookup(static_cast<IExpression*>(assignment))
        ->Assign(assignment->mName, mResult);
}

void Interpreter::Visit(ExpressionCall* call)
{
    Object callee { Evaluate(call->mCallee.get()) };

    std::vector<Object> arguments;
    for (auto& expr : call->mArguments) {
        arguments.push_back(Evaluate(expr.get()));
    }

    if (!std::holds_alternative<std::shared_ptr<ICallable>>(callee.mData)) {
        throw InterpreterException("Callee must be a callable function");
    }

    std::shared_ptr<ICallable> callable = std::get<std::shared_ptr<ICallable>>(callee.mData);

    if (arguments.size() != callable->Arity()) {
        throw InterpreterException(fmt::format("Expected {} arguments but received {}", callable->Arity(), arguments.size()));
    }

    mResult = callable->Call(this, arguments);
}

void Interpreter::Visit(ExpressionGet* get)
{
    Object object = Evaluate(get->mObject.get());
    if (!std::holds_alternative<std::shared_ptr<Instance>>(object.mData)) {
        throw InterpreterException("Cannot get, only instances have properties");
    }

    mResult = std::get<std::shared_ptr<Instance>>(object.mData)->Get(get->mName);
}

void Interpreter::Visit(ExpressionSet* set)
{
    Object object = Evaluate(set->mObject.get());
    if (!std::holds_alternative<std::shared_ptr<Instance>>(object.mData)) {
        throw InterpreterException("Cannot set, only instances have properties");
    }

    Object value = Evaluate(set->mValue.get());

    std::get<std::shared_ptr<Instance>>(object.mData)->Set(set->mName, value);
}

void Interpreter::Visit(ExpressionThis* expressionThis)
{
    mResult = ResolutionLookup(expressionThis)->Get("this");
}

}
