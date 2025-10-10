#include "interpreter.h"
#include "ast.h"
#include "function.h"
#include "icallable.h"
#include "object.h"
#include "parser.h"
#include "token.h"

#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <variant>
#include <vector>

namespace cpplox {

Interpreter::Interpreter(std::vector<std::unique_ptr<IStatement>>&& statements)
    : mStatements { std::move(statements) }
    , mEnvironment { std::make_unique<Environment>() }
{
}

void Interpreter::Run()
{
    for (auto& statement : mStatements) {
        if (statement != nullptr)
            statement->Accept(this);
    }
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
    if (variable->mInitializer.has_value()) {
        initializer = Evaluate(variable->mInitializer.value().get());
    }
    mEnvironment->Define(variable->mName->mLexeme, initializer);
}

void Interpreter::Visit(StatementBlock* block)
{
    std::unique_ptr<Environment> oldEnvironment = std::move(mEnvironment);
    mEnvironment = std::make_unique<Environment>(oldEnvironment.get());
    for (auto& statement : block->mBlock) {
        if (statement != nullptr)
            statement->Accept(this);
    }
    mEnvironment = std::move(oldEnvironment);
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
    mEnvironment->Define(function->mIdentifier->mLexeme,
        Object(std::static_pointer_cast<ICallable>(std::make_shared<Function>(fmt::format("<fun {}>", function->mIdentifier->mLexeme),
            std::move(function->mParameters), function->mParameters.size(), std::move(function->mBody)))));
}

void Interpreter::Visit(StatementReturn* returnStatement)
{
    if (returnStatement->mExpression != nullptr) {
        returnStatement->mExpression->Accept(this);
    }
    throw ReturnException {};
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

    switch (unary->mOperator->mType) {
    case Token::Type::kMinus:
        AssertType<double>(mResult);
        mResult.mData = -std::get<double>(mResult.mData);
        break;

    case Token::Type::kBang:
        mResult = Object(IsTrue(mResult));
        break;

    default:
        throw ParserException("Interpreter internal error while interpreting type ExpressionUnary");
    }
}

void Interpreter::Visit(ExpressionLogical* logical)
{
    Object left = Evaluate(logical->mLeft.get());

    switch (logical->mOperator->mType) {
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
        throw ParserException("Interpreter internal error while interpreting ExpressionLogical");
    }
}

void Interpreter::Visit(ExpressionBinary* binary)
{
    Object left = Evaluate(binary->mLeft.get());
    Object right = Evaluate(binary->mRight.get());

    switch (binary->mOperator->mType) {
    case Token::Type::kMinus:
        AssertType<double>(left, right);
        mResult = Object(std::get<double>(left.mData) - std::get<double>(right.mData));
        break;

    case Token::Type::kSlash:
        AssertType<double>(left, right);
        if (std::get<double>(right.mData) == 0) {
            throw ParserException("Divide by zero");
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
        throw ParserException("Interpreter internal error while interpreting type ExpressionBinary");
    }
}

void Interpreter::Visit(ExpressionVariable* variable)
{
    mResult = *mEnvironment->Get(variable->mName->mLexeme);
}

void Interpreter::Visit(ExpressionAssignment* assignment)
{
    mResult = Evaluate(assignment->mValue.get());
    mEnvironment->Assign(assignment->mName->mLexeme, mResult);
}

void Interpreter::Visit(ExpressionCall* call)
{
    Object callee { Evaluate(call->mCallee.get()) };

    std::vector<Object> arguments;
    for (auto& expr : call->mArguments) {
        arguments.push_back(Evaluate(expr.get()));
    }

    if (!std::holds_alternative<std::shared_ptr<ICallable>>(callee.mData)) {
        throw ParserException("Callee must be a callable function");
    }

    ICallable* function = std::get<std::shared_ptr<ICallable>>(callee.mData).get();

    if (arguments.size() != function->Arity()) {
        throw ParserException(fmt::format("Expected {} arguments but received {}", function->Arity(), arguments.size()));
    }

    mResult = function->Call(this, arguments);
}

}
