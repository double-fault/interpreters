#include "resolver.h"
#include "ast.h"
#include "interpreter.h"

#include <ranges>
#include <spdlog/spdlog.h>

namespace cpplox {

Resolver::Resolver(Interpreter* interpreter,
    const std::vector<IStatement*>& statements)
    : mInterpreter { interpreter }
    , mStatements { statements }
    , mCurrentFunction { FunctionType::kNone }
    , mCurrentClass { ClassType::kNone }
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

void Resolver::Declare(const std::string& name)
{
    if (!mScopes.empty()) {
        auto& scope { mScopes.back() };
        if (scope.find(name) != scope.end()) {
            throw ResolverException(fmt::format("Variable with name '{}' already exists in scope",
                name));
        }
        scope[name] = false;
    }
}

void Resolver::Define(const std::string& name)
{
    if (!mScopes.empty())
        mScopes.back()[name] = true;
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
    Declare(variable->mName);
    if (variable->mInitializer != nullptr) {
        variable->mInitializer->Accept(this);
    }
    Define(variable->mName);
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
    Declare(function->mIdentifier);
    Define(function->mIdentifier);

    StartScope();
    for (auto& parameter : function->mParameters) {
        Declare(parameter);
        Define(parameter);
    }

    FunctionType oldType = mCurrentFunction;
    mCurrentFunction = FunctionType::kFunction;
    if (oldType == FunctionType::kMethod) {
        if (function->mIdentifier != "init") {
            mCurrentFunction = FunctionType::kMethod;
        } else {
            mCurrentFunction = FunctionType::kInitializer;
        }
    }
    for (auto& statement : function->mBody) {
        statement->Accept(this);
    }
    mCurrentFunction = oldType;

    EndScope();
}

void Resolver::Visit(StatementReturn* statement)
{
    if (mCurrentFunction == FunctionType::kInitializer) {
        throw ResolverException("Cannot return fron constructor");
    }

    if (mCurrentFunction != FunctionType::kFunction && mCurrentFunction != FunctionType::kMethod) {
        throw ResolverException("Cannot return from outside a function");
    }

    if (statement->mExpression != nullptr) {
        statement->mExpression->Accept(this);
    }
}

void Resolver::Visit(StatementClass* klass)
{
    Declare(klass->mIdentifier);
    Define(klass->mIdentifier);

    ClassType oldClassType = mCurrentClass;
    FunctionType oldFunctionType = mCurrentFunction;
    mCurrentClass = ClassType::kClass;
    mCurrentFunction = FunctionType::kMethod;

    StartScope();
    StartScope();
    Declare("this");
    Define("this");

    for (auto& method : klass->mMethods) {
        method->Accept(this);
    }

    EndScope();
    EndScope();

    mCurrentClass = oldClassType;
    mCurrentFunction = oldFunctionType;
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
    std::string name = variable->mName;
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
    std::string name = assignment->mName;
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

void Resolver::Visit(ExpressionGet* get)
{
    get->mObject->Accept(this);
}

void Resolver::Visit(ExpressionSet* set)
{
    set->mObject->Accept(this);
    set->mValue->Accept(this);
}

void Resolver::Visit(ExpressionThis* expression)
{
    if (mCurrentFunction != FunctionType::kMethod && mCurrentFunction != FunctionType::kInitializer) {
        throw ResolverException("Keyword 'this' can only be used inside methods");
    }

    int depth { 0 };
    for (auto& scope : std::ranges::views::reverse(mScopes)) {
        if (scope.find("this") != scope.end()) {
            mInterpreter->Resolve(expression, depth);
            break;
        }
        depth++;
    }
}

}
