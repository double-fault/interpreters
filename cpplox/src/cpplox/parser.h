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

    std::vector<std::unique_ptr<IStatement>> Parse();

private:
    std::unique_ptr<IStatement> Declaration();
    std::unique_ptr<IStatement> DeclarationVariable();
    std::unique_ptr<IStatement> Statement();
    std::unique_ptr<IStatement> Block();

    std::unique_ptr<IExpression> Expression();
    std::unique_ptr<IExpression> Assignment();
    std::unique_ptr<IExpression> Equality();
    std::unique_ptr<IExpression> Comparison();
    std::unique_ptr<IExpression> Term();
    std::unique_ptr<IExpression> Factor();
    std::unique_ptr<IExpression> Unary();
    std::unique_ptr<IExpression> Primary();

    bool Match(std::initializer_list<Token::Type>);
    Token* Next();
    Token* Peek();

    std::vector<Token> mTokens;
    std::vector<Token>::iterator mIterator { mTokens.begin() };
};

}
