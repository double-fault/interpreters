#pragma once

#include "object.h"
#include "token.h"

#include <cassert>
#include <memory>
#include <vector>

// TODO: For proper error handling, you'd want to note the location of the corresponding token
// with each AST node. Create some small new struct and add it as a field to every AST node?
// Anything requiring less labor? Like some neat way? Hmm

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
class StatementClass;

class ExpressionBinary;
class ExpressionLogical;
class ExpressionGrouping;
class ExpressionObject;
class ExpressionUnary;
class ExpressionVariable;
class ExpressionAssignment;
class ExpressionCall;
class ExpressionGet;
class ExpressionSet;
class ExpressionThis;

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
    virtual void Visit(StatementClass*) = 0;

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
    virtual void Visit(ExpressionGet*) = 0;
    virtual void Visit(ExpressionSet*) = 0;
    virtual void Visit(ExpressionThis*) = 0;

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
    StatementVariable(const std::string& name, std::unique_ptr<IExpression> initializer = nullptr)
        : mName { name }
        , mInitializer { std::move(initializer) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    const std::string mName;
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
    StatementFunction(const std::string& identifier, std::vector<std::string> parameters,
        std::vector<std::unique_ptr<IStatement>> body)
        : mIdentifier { identifier }
        , mParameters { parameters }
        , mBody { std::move(body) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    const std::string mIdentifier;
    std::vector<std::string> mParameters;
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

class StatementClass final : public IStatement {
public:
    StatementClass(const std::string& identifier, std::unique_ptr<IExpression> superclass,
        std::vector<std::unique_ptr<IStatement>> methods)
        : mIdentifier { identifier }
        , mSuperclass { std::move(superclass) }
        , mMethods { std::move(methods) }
    {
    }

    void Accept(IStatementVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    const std::string mIdentifier;
    std::unique_ptr<IExpression> mSuperclass;
    std::vector<std::unique_ptr<IStatement>> mMethods;
};

class ExpressionBinary final : public IExpression {
public:
    ExpressionBinary(std::unique_ptr<IExpression> left, const Token::Type op, std::unique_ptr<IExpression> right)
        : mLeft { std::move(left) }
        , mOperator { op }
        , mRight { std::move(right) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mLeft;
    const Token::Type mOperator;
    std::unique_ptr<IExpression> mRight;
};

class ExpressionLogical final : public IExpression {
public:
    ExpressionLogical(std::unique_ptr<IExpression> left, const Token::Type op, std::unique_ptr<IExpression> right)
        : mLeft { std::move(left) }
        , mOperator { op }
        , mRight { std::move(right) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mLeft, mRight;
    const Token::Type mOperator;
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
    ExpressionUnary(const Token::Type op, std::unique_ptr<IExpression> expression)
        : mOperator { op }
        , mExpression { std::move(expression) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    const Token::Type mOperator;
    std::unique_ptr<IExpression> mExpression;
};

class ExpressionVariable final : public IExpression {
public:
    ExpressionVariable(const std::string& name)
        : mName { name }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    const std::string mName;
};

class ExpressionAssignment final : public IExpression {
public:
    ExpressionAssignment(const std::string& name, std::unique_ptr<IExpression> value)
        : mName { name }
        , mValue { std::move(value) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    const std::string mName;
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

class ExpressionGet final : public IExpression {
public:
    ExpressionGet(std::unique_ptr<IExpression> object, const std::string& name)
        : mObject { std::move(object) }
        , mName { name }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mObject;
    const std::string mName;
};

class ExpressionSet final : public IExpression {
public:
    ExpressionSet(std::unique_ptr<IExpression> object, const std::string& name,
        std::unique_ptr<IExpression> value)
        : mObject { std::move(object) }
        , mName { name }
        , mValue { std::move(value) }
    {
    }

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }

    std::unique_ptr<IExpression> mObject;
    const std::string mName;
    std::unique_ptr<IExpression> mValue;
};

class ExpressionThis final : public IExpression {
public:
    ExpressionThis() = default;

    void Accept(IExpressionVisitor* visitor) override
    {
        visitor->Visit(this);
    }
};

}
