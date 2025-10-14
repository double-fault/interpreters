#pragma once

#include "ast.h"
#include "environment.h"
#include "object.h"
#include "parser.h"

#include <boost/type_index.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace cpplox {

class InterpreterException final : public std::runtime_error {
public:
    InterpreterException(const std::string& error)
        : std::runtime_error { error }
    {
    }
};

class Interpreter final : public IExpressionVisitor,
                          public IStatementVisitor {
public:
    class ReturnException final : public std::runtime_error {
    public:
        ReturnException()
            : std::runtime_error { "interpreter return exception - is always caught, you should not be seeing this" }
        {
        }
    };

    Interpreter(const std::vector<std::shared_ptr<IStatement>>&);
    void Run();

    void Resolve(IExpression*, int);
    Environment* ResolutionLookup(IExpression*);

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

    Object GetResult();

    std::shared_ptr<Environment> mGlobals;
    std::shared_ptr<Environment> mEnvironment;

private:
    Object Evaluate(IExpression*);
    bool IsTrue(const Object&);
    bool IsEqual(const Object&, const Object&);

    template <typename... T>
    void AssertTypeSingle(const Object& object)
    {
        if (!(std::holds_alternative<T>(object.mData) || ...))
            throw ParserException(fmt::format("Expected one of the types = {}",
                fmt::join(std::vector<std::string> {
                              boost::typeindex::type_id<T>().pretty_name()... },
                    ",")));
    }

    template <typename... T, typename... Args>
    void AssertType(const Args&... objects)
    {
        ((void)AssertTypeSingle<T...>(objects), ...);
    }

    std::vector<std::shared_ptr<IStatement>> mStatements;
    std::map<IExpression*, int> mResolvedLocals;

    Object mResult;
};

}
