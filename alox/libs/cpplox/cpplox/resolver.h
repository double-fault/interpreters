#pragma once

#include "ast.h"
#include "interpreter.h"

namespace cpplox {

class ResolverException final : public std::runtime_error {
public:
    ResolverException(const std::string& error)
        : std::runtime_error { error }
    {
    }
};

// Note: Resolver should not modify the passed AST
class Resolver final : public IExpressionVisitor,
                       public IStatementVisitor {
public:
    enum class FunctionType {
        kNone = 0,
        kFunction,
        kMethod,
        kInitializer
    };

    enum class ClassType {
        kNone = 0,
        kClass
    };

    Resolver(Interpreter*, const std::vector<IStatement*>&);
    void Resolve();

    void Visit(IStatement*) override;
    void Visit(StatementExpression*) override;
    void Visit(StatementPrint*) override;
    void Visit(StatementVariable*) override;
    void Visit(StatementBlock*) override;
    void Visit(StatementIf*) override;
    void Visit(StatementWhile*) override;
    void Visit(StatementFunction*) override;
    void Visit(StatementReturn*) override;
    void Visit(StatementClass*) override;

    void Visit(IExpression*) override;
    void Visit(ExpressionBinary*) override;
    void Visit(ExpressionLogical*) override;
    void Visit(ExpressionGrouping*) override;
    void Visit(ExpressionObject*) override;
    void Visit(ExpressionUnary*) override;
    void Visit(ExpressionVariable*) override;
    void Visit(ExpressionAssignment*) override;
    void Visit(ExpressionCall*) override;
    void Visit(ExpressionGet*) override;
    void Visit(ExpressionSet*) override;
    void Visit(ExpressionThis*) override;

private:
    void StartScope();
    void Declare(const std::string& name);
    void Define(const std::string& name);
    void EndScope();

    Interpreter* mInterpreter;
    std::vector<IStatement*> mStatements;
    std::vector<std::map<std::string, bool>> mScopes {};
    FunctionType mCurrentFunction;
    ClassType mCurrentClass;
};

}
