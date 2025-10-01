#include "interpreter.h"
#include "ast.h"
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
    if (object.mType == Object::Type::kNil)
        return false;
    if (object.mType == Object::Type::kBool)
        return std::get<bool>(object.mData);
    return true;
}

bool Interpreter::IsEqual(const Object& a, const Object& b)
{
    if (a.mType == Object::Type::kNil || b.mType == Object::Type::kNil) {
        return a.mType == b.mType;
    }

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
    mEnvironment = std::make_unique<Environment>(std::move(mEnvironment));
    for (auto& statement : block->mBlock) {
        if (statement != nullptr) 
            statement->Accept(this);
    }
    mEnvironment = std::move(mEnvironment->mEnclosingEnvironment);
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

}
