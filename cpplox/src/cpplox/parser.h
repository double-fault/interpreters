#pragma once

#include "ast.h"
#include "token.h"

#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <vector>

namespace cpplox {

class ParserException final : public std::runtime_error {
public:
    ParserException(const std::string& error)
        : std::runtime_error { error }
    {
    }
};

class Parser final {
public:
    Parser(const std::vector<Token>&);

    std::vector<std::shared_ptr<IStatement>> Parse();

private:
    std::unique_ptr<IStatement> Declaration();
    std::unique_ptr<IStatement> DeclarationVariable();
    std::unique_ptr<IStatement> DeclarationFunction();
    std::unique_ptr<IStatement> Statement();
    std::unique_ptr<IStatement> ExpressionStatement();
    std::unique_ptr<IStatement> If();
    std::unique_ptr<IStatement> While();
    std::unique_ptr<IStatement> For();
    std::unique_ptr<IStatement> Return();

    std::unique_ptr<IExpression> Expression();
    std::unique_ptr<IExpression> Assignment();
    std::unique_ptr<IExpression> LogicalOr();
    std::unique_ptr<IExpression> LogicalAnd();
    std::unique_ptr<IExpression> Equality();
    std::unique_ptr<IExpression> Comparison();
    std::unique_ptr<IExpression> Term();
    std::unique_ptr<IExpression> Factor();
    std::unique_ptr<IExpression> Unary();
    std::unique_ptr<IExpression> Call();
    std::unique_ptr<IExpression> Primary();

    bool Match(std::initializer_list<Token::Type>);
    Token* Next();
    Token* Peek();

    std::vector<Token> mTokens;
    std::vector<Token>::iterator mIterator { mTokens.begin() };
};

}
