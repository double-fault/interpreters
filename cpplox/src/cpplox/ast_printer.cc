#include "ast_printer.h"
#include "ast.h"

namespace cpplox {

void AstPrinter::Visit(IExpression* expression)
{
    expression->Accept(this);
}

void AstPrinter::Visit(ExpressionBinary* binary)
{
    ToString(binary->mLeft.get(), binary->mOperator->mLexeme, binary->mRight.get());
}

void AstPrinter::Visit(ExpressionGrouping* grouping)
{
    grouping->mExpression->Accept(this);
    mResult = "(" + mResult + ")";
}

void AstPrinter::Visit(ExpressionObject* object)
{
    // TODO: Handle nil?
    mResult = object->mObject.ToString();
}

void AstPrinter::Visit(ExpressionUnary* unary)
{
    ToString(unary->mOperator->mLexeme, unary->mExpression.get());
}

void AstPrinter::ToString(const std::string& str, IExpression* expr) 
{
    expr->Accept(this);
    mResult = "(" + str + mResult + ")";
}

std::string AstPrinter::GetResult()
{
    return mResult;
}

void AstPrinter::ToString(IExpression* exprLeft, const std::string& str, IExpression* exprRight)
{
    std::string ret { "(" };

    exprLeft->Accept(this);
    ret += mResult + str;

    exprRight->Accept(this);
    ret += mResult;

    mResult = ret + ")";
}

}
