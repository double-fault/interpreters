#pragma once

#include "ast.h"

namespace cpplox {

class AstPrinter final : public IExpressionVisitor {
public:
    void Visit(ExpressionBinary*) override;
    void Visit(ExpressionGrouping*) override;
    void Visit(ExpressionLiteral*) override;
    void Visit(ExpressionUnary*) override;

    std::string GetResult();

private:
    void ToString(const std::string& str, IExpression* expr);
    void ToString(IExpression* exprLeft, const std::string& str, IExpression* exprRight);

    std::string mResult;
};

}
