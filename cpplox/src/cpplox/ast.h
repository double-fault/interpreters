#pragma once

#include "object.h"
#include "token.h"

#include <memory>

namespace cpplox {

class IExpression;
class IExpressionVisitor;

class ExpressionBinary;
class ExpressionGrouping;
class ExpressionObject;
class ExpressionUnary;

class IExpression {
public:
    virtual void Accept(IExpressionVisitor* visitor) = 0;

    virtual ~IExpression() = default;
};

class IExpressionVisitor {
public:
    virtual void Visit(IExpression*) = 0;
    virtual void Visit(ExpressionBinary*) = 0;
    virtual void Visit(ExpressionGrouping*) = 0;
    virtual void Visit(ExpressionObject*) = 0;
    virtual void Visit(ExpressionUnary*) = 0;

    virtual ~IExpressionVisitor() = default;
};

class ExpressionBinary final : public IExpression {
public:
    ExpressionBinary(std::unique_ptr<IExpression> left, std::unique_ptr<Token> op, std::unique_ptr<IExpression> right)
        : mLeft { std::move(left) }
        , mOperator { std::move(op) }
        , mRight { std::move(right) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mLeft;
    std::unique_ptr<Token> mOperator;
    std::unique_ptr<IExpression> mRight;
};

class ExpressionGrouping final : public IExpression {
public:
    ExpressionGrouping(std::unique_ptr<IExpression> expression)
        : mExpression { std::move(expression) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mExpression;
};

class ExpressionObject final : public IExpression {
public:
    ExpressionObject(const Object& object)
        : mObject { object }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    Object mObject;
};

class ExpressionUnary final : public IExpression {
public:
    ExpressionUnary(std::unique_ptr<Token> op, std::unique_ptr<IExpression> expression)
        : mOperator { std::move(op) }
        , mExpression { std::move(expression) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<Token> mOperator;
    std::unique_ptr<IExpression> mExpression;
};

}
