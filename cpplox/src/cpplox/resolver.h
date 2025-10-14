#pragma once

#include "ast.h"
#include "interpreter.h"

#include <memory>

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
        kFunction
    };

    Resolver(Interpreter*, const std::vector<std::shared_ptr<IStatement>>&);
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

    void Visit(IExpression*) override;
    void Visit(ExpressionBinary*) override;
    void Visit(ExpressionLogical*) override;
    void Visit(ExpressionGrouping*) override;
    void Visit(ExpressionObject*) override;
    void Visit(ExpressionUnary*) override;
    void Visit(ExpressionVariable*) override;
    void Visit(ExpressionAssignment*) override;
    void Visit(ExpressionCall*) override;

private:
    void StartScope();
    void Declare(Token* identifier);
    void Define(Token* identifier);
    void EndScope();

    Interpreter* mInterpreter;
    std::vector<std::shared_ptr<IStatement>> mStatements;
    std::vector<std::map<std::string, bool>> mScopes {};
    FunctionType mCurrentFunction;
};

}
