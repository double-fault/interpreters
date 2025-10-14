#include "resolver.h"
#include "ast.h"
#include "interpreter.h"

#include <ranges>
#include <spdlog/spdlog.h>

namespace cpplox {

Resolver::Resolver(Interpreter* interpreter,
    const std::vector<std::shared_ptr<IStatement>>& statements)
    : mInterpreter { interpreter }
    , mStatements { statements }
    , mCurrentFunction { FunctionType::kNone }
{
}

void Resolver::Resolve()
{
    for (auto& statement : mStatements) {
        statement->Accept(this);
    }
}

void Resolver::StartScope()
{
    mScopes.push_back(std::map<std::string, bool> {});
}

void Resolver::Declare(Token* identifier)
{
    if (!mScopes.empty()) {
        auto& scope { mScopes.back() };
        if (scope.find(identifier->mLexeme) != scope.end()) {
            throw ResolverException(fmt::format("Variable with name '{}' already exists in scope",
                identifier->mLexeme));
        }
        scope[identifier->mLexeme] = false;
    }
}

void Resolver::Define(Token* identifier)
{
    if (!mScopes.empty())
        mScopes.back()[identifier->mLexeme] = true;
}

void Resolver::EndScope()
{
    if (mScopes.empty()) {
        throw ResolverException("Cannot end global scope");
    }
    mScopes.pop_back();
}

void Resolver::Visit(IStatement* statement)
{
    statement->Accept(this);
}

void Resolver::Visit(StatementExpression* expression)
{
    expression->mExpression->Accept(this);
}

void Resolver::Visit(StatementPrint* print)
{
    print->mExpression->Accept(this);
}

void Resolver::Visit(StatementVariable* variable)
{
    Declare(variable->mName.get());
    if (variable->mInitializer != nullptr) {
        variable->mInitializer->Accept(this);
    }
    Define(variable->mName.get());
}

void Resolver::Visit(StatementBlock* block)
{
    StartScope();
    for (auto& statement : block->mStatements) {
        statement->Accept(this);
    }
    EndScope();
}

void Resolver::Visit(StatementIf* statement)
{
    statement->mCondition->Accept(this);
    statement->mThenStatement->Accept(this);

    if (statement->mElseStatement != nullptr) {
        statement->mElseStatement->Accept(this);
    }
}

void Resolver::Visit(StatementWhile* statement)
{
    statement->mCondition->Accept(this);
    statement->mBody->Accept(this);
}

void Resolver::Visit(StatementFunction* function)
{
    Declare(function->mIdentifier.get());
    Define(function->mIdentifier.get());

    StartScope();
    for (auto& parameter : function->mParameters) {
        Declare(parameter.get());
        Define(parameter.get());
    }

    FunctionType oldType = mCurrentFunction;
    mCurrentFunction = FunctionType::kFunction;
    for (auto& statement : function->mBody) {
        statement->Accept(this);
    }
    mCurrentFunction = oldType;

    EndScope();
}

void Resolver::Visit(StatementReturn* statement)
{
    if (mCurrentFunction != FunctionType::kFunction) {
        throw ResolverException("Cannot return from outside a function");
    }

    if (statement->mExpression != nullptr) {
        statement->mExpression->Accept(this);
    }
}

void Resolver::Visit(IExpression* expression)
{
    expression->Accept(this);
}

void Resolver::Visit(ExpressionBinary* binary)
{
    binary->mLeft->Accept(this);
    binary->mRight->Accept(this);
}

void Resolver::Visit(ExpressionLogical* logical)
{
    logical->mLeft->Accept(this);
    logical->mRight->Accept(this);
}

void Resolver::Visit(ExpressionGrouping* grouping)
{
    grouping->mExpression->Accept(this);
}

void Resolver::Visit(ExpressionObject* object)
{
}

void Resolver::Visit(ExpressionUnary* unary)
{
    unary->mExpression->Accept(this);
}

void Resolver::Visit(ExpressionVariable* variable)
{
    std::string name = variable->mName->mLexeme;
    int depth { 0 };
    for (auto& scope : std::ranges::views::reverse(mScopes)) {
        if (scope.find(name) != scope.end()) {
            if (!scope[name]) {
                throw ResolverException("Cannot use variable in its own initializer");
            }
            mInterpreter->Resolve(variable, depth);
            break;
        }
        depth++;
    }
}

void Resolver::Visit(ExpressionAssignment* assignment)
{
    std::string name = assignment->mName->mLexeme;
    int depth { 0 };
    for (auto& scope : std::ranges::views::reverse(mScopes)) {
        if (scope.find(name) != scope.end()) {
            if (!scope[name]) {
                throw ResolverException("Cannot use variable in its own initializer");
            }
            mInterpreter->Resolve(assignment, depth);
            break;
        }
        depth++;
    }

    assignment->mValue->Accept(this);
}

void Resolver::Visit(ExpressionCall* call)
{
    call->mCallee->Accept(this);
    for (auto& argument : call->mArguments) {
        argument->Accept(this);
    }
}

}
