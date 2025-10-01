#pragma once

#include "object.h"
#include "token.h"

#include <memory>
#include <optional>
#include <vector>

namespace cpplox {

class IExpression;
class IExpressionVisitor;

class IStatement;
class IStatementVisitor;

class StatementExpression;
class StatementPrint;
class StatementVariable;
class StatementBlock;

class ExpressionBinary;
class ExpressionGrouping;
class ExpressionObject;
class ExpressionUnary;
class ExpressionVariable;
class ExpressionAssignment;

class IStatement {
public:
    virtual void Accept(IStatementVisitor* visitor) = 0;

    virtual ~IStatement() = default;
};

class IStatementVisitor {
public:
    virtual void Visit(IStatement*) = 0;
    virtual void Visit(StatementExpression*) = 0;
    virtual void Visit(StatementPrint*) = 0;
    virtual void Visit(StatementVariable*) = 0;
    virtual void Visit(StatementBlock*) = 0;

    virtual ~IStatementVisitor() = default;
};

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
    virtual void Visit(ExpressionVariable*) = 0;
    virtual void Visit(ExpressionAssignment*) = 0;

    virtual ~IExpressionVisitor() = default;
};

class StatementExpression final : public IStatement {
public:
    StatementExpression(std::unique_ptr<IExpression> expression)
        : mExpression { std::move(expression) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mExpression;
};

class StatementPrint final : public IStatement {
public:
    StatementPrint(std::unique_ptr<IExpression> expression)
        : mExpression { std::move(expression) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mExpression;
};

class StatementVariable final : public IStatement {
public:
    StatementVariable(std::unique_ptr<Token> name, std::optional<std::unique_ptr<IExpression>> initializer = std::nullopt)
        : mName { std::move(name) }
        , mInitializer { std::move(initializer) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<Token> mName;
    std::optional<std::unique_ptr<IExpression>> mInitializer;
};

class StatementBlock final : public IStatement {
public:
    StatementBlock(std::vector<std::unique_ptr<IStatement>> block)
        : mBlock { std::move(block) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::vector<std::unique_ptr<IStatement>> mBlock;
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

class ExpressionVariable final : public IExpression {
public:
    ExpressionVariable(std::unique_ptr<Token> name)
        : mName { std::move(name) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<Token> mName;
};

class ExpressionAssignment final : public IExpression {
public:
    ExpressionAssignment(std::unique_ptr<Token> name, std::unique_ptr<IExpression> value)
        : mName { std::move(name) }
        , mValue { std::move(value) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<Token> mName;
    std::unique_ptr<IExpression> mValue;
};

}
