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

class Interpreter : public IExpressionVisitor, public IStatementVisitor {
public:
    Interpreter(std::vector<std::unique_ptr<IStatement>>&&);
    void Run();

    void Visit(IStatement*) override;
    void Visit(StatementExpression*) override;
    void Visit(StatementPrint*) override;
    void Visit(StatementVariable*) override;
    void Visit(StatementBlock*) override;
    void Visit(StatementIf*) override;
    void Visit(StatementWhile*) override;

    void Visit(IExpression*) override;
    void Visit(ExpressionBinary*) override;
    void Visit(ExpressionLogical*) override;
    void Visit(ExpressionGrouping*) override;
    void Visit(ExpressionObject*) override;
    void Visit(ExpressionUnary*) override;
    void Visit(ExpressionVariable*) override;
    void Visit(ExpressionAssignment*) override;

    Object GetResult();

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

    std::unique_ptr<Environment> mEnvironment;
    std::vector<std::unique_ptr<IStatement>> mStatements;
    
    Object mResult;
};

}
