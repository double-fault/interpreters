#pragma once

#include "object.h"
#include "token.h"

#include <cassert>
#include <memory>
#include <vector>

// TODO: Replace passing Tokens into AST nodes with more specific items - variable name (std::string), operator type, etc.

namespace cpplox {

class IExpression;
class IExpressionVisitor;

class IStatement;
class IStatementVisitor;

class StatementExpression;
class StatementPrint;
class StatementVariable;
class StatementBlock;
class StatementIf;
class StatementWhile;
class StatementFunction;
class StatementReturn;

class ExpressionBinary;
class ExpressionLogical;
class ExpressionGrouping;
class ExpressionObject;
class ExpressionUnary;
class ExpressionVariable;
class ExpressionAssignment;
class ExpressionCall;

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
    virtual void Visit(StatementIf*) = 0;
    virtual void Visit(StatementWhile*) = 0;
    virtual void Visit(StatementFunction*) = 0;
    virtual void Visit(StatementReturn*) = 0;

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
    virtual void Visit(ExpressionLogical*) = 0;
    virtual void Visit(ExpressionGrouping*) = 0;
    virtual void Visit(ExpressionObject*) = 0;
    virtual void Visit(ExpressionUnary*) = 0;
    virtual void Visit(ExpressionVariable*) = 0;
    virtual void Visit(ExpressionAssignment*) = 0;
    virtual void Visit(ExpressionCall*) = 0;

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
    StatementVariable(std::unique_ptr<Token> name, std::unique_ptr<IExpression> initializer = nullptr)
        : mName { std::move(name) }
        , mInitializer { std::move(initializer) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<Token> mName;
    std::unique_ptr<IExpression> mInitializer;
};

class StatementBlock final : public IStatement {
public:
    StatementBlock(std::vector<std::unique_ptr<IStatement>> block)
        : mStatements { std::move(block) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::vector<std::unique_ptr<IStatement>> mStatements;
};

class StatementIf final : public IStatement {
public:
    StatementIf(std::unique_ptr<IExpression> condition, std::unique_ptr<IStatement> thenStatement,
        std::unique_ptr<IStatement> elseStatement)
        : mCondition { std::move(condition) }
        , mThenStatement { std::move(thenStatement) }
        , mElseStatement { std::move(elseStatement) }
    {
        assert(mThenStatement != nullptr);
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mCondition;
    std::unique_ptr<IStatement> mThenStatement;
    std::unique_ptr<IStatement> mElseStatement;
};

class StatementWhile final : public IStatement {
public:
    StatementWhile(std::unique_ptr<IExpression> condition, std::unique_ptr<IStatement> body)
        : mCondition { std::move(condition) }
        , mBody { std::move(body) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mCondition;
    std::unique_ptr<IStatement> mBody;
};

class StatementFunction final : public IStatement {
public:
    StatementFunction(std::unique_ptr<Token> identifier, std::vector<std::unique_ptr<Token>> parameters,
        std::vector<std::unique_ptr<IStatement>> body)
        : mIdentifier { std::move(identifier) }
        , mParameters { std::move(parameters) }
        , mBody { std::move(body) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<Token> mIdentifier;
    std::vector<std::unique_ptr<Token>> mParameters;
    std::vector<std::unique_ptr<IStatement>> mBody;
};

class StatementReturn final : public IStatement {
public:
    StatementReturn()
        : mExpression { nullptr }
    {
    }

    StatementReturn(std::unique_ptr<IExpression> expression)
        : mExpression { std::move(expression) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mExpression;
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

class ExpressionLogical final : public IExpression {
public:
    ExpressionLogical(std::unique_ptr<IExpression> left, std::unique_ptr<Token> op, std::unique_ptr<IExpression> right)
        : mLeft { std::move(left) }
        , mOperator { std::move(op) }
        , mRight { std::move(right) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mLeft, mRight;
    std::unique_ptr<Token> mOperator;
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

class ExpressionCall final : public IExpression {
public:
    ExpressionCall(std::unique_ptr<IExpression> callee, std::vector<std::unique_ptr<IExpression>> arguments)
        : mCallee { std::move(callee) }
        , mArguments { std::move(arguments) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mCallee;
    std::vector<std::unique_ptr<IExpression>> mArguments;
};

}
