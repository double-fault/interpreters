#include "parser.h"
#include "ast.h"
#include "object.h"
#include "token.h"

#include <initializer_list>
#include <memory>
#include <spdlog/spdlog.h>

namespace cpplox {

Parser::Parser(const std::vector<Token>& tokens)
    : mTokens { tokens }
{
    mTokens.pop_back(); // Get rid of EOF token
}

std::unique_ptr<IExpression> Parser::Parse()
{
    return Expression();
}

std::unique_ptr<IExpression> Parser::Expression()
{
    return Equality();
}

std::unique_ptr<IExpression> Parser::Equality()
{
    std::unique_ptr<IExpression> expression = Comparison();

    while (Match({ Token::Type::kBangEqual, Token::Type::kEqualEqual })) {
        std::unique_ptr<Token> op { std::make_unique<Token>(*Peek()) };
        Next();
        expression = std::make_unique<ExpressionBinary>(std::move(expression),
            std::move(op), Comparison());
    }
    return expression;
}

std::unique_ptr<IExpression> Parser::Comparison()
{
    std::unique_ptr<IExpression> expression = Term();

    while (Match({ Token::Type::kLess, Token::Type::kLessEqual,
        Token::Type::kGreater, Token::Type::kGreaterEqual })) {
        std::unique_ptr<Token> op { std::make_unique<Token>(*Peek()) };
        Next();
        expression = std::make_unique<ExpressionBinary>(std::move(expression), std::move(op), Term());
    }
    return expression;
}

std::unique_ptr<IExpression> Parser::Term()
{
    std::unique_ptr<IExpression> expression = Factor();

    while (Match({ Token::Type::kPlus, Token::Type::kMinus })) {
        std::unique_ptr<Token> op { std::make_unique<Token>(*Peek()) };
        Next();
        expression = std::make_unique<ExpressionBinary>(std::move(expression), std::move(op), Factor());
    }
    return expression;
}

std::unique_ptr<IExpression> Parser::Factor()
{
    std::unique_ptr<IExpression> expression = Unary();

    while (Match({ Token::Type::kStar, Token::Type::kSlash })) {
        std::unique_ptr<Token> op { std::make_unique<Token>(*Peek()) };
        Next();
        expression = std::make_unique<ExpressionBinary>(std::move(expression), std::move(op), Unary());
    }
    return expression;
}

std::unique_ptr<IExpression> Parser::Unary()
{
    if (Match({ Token::Type::kMinus, Token::Type::kBang })) {
        std::unique_ptr<Token> op { std::make_unique<Token>(*Peek()) };
        Next();
        return std::make_unique<ExpressionUnary>(std::move(op), Unary());
    }
    return Primary();
}

std::unique_ptr<IExpression> Parser::Primary()
{
    if (Match({ Token::Type::kFalse })) {
        Next();
        return std::make_unique<ExpressionObject>(Object(false));
    }
    if (Match({ Token::Type::kTrue })) {
        Next();
        return std::make_unique<ExpressionObject>(Object(true));
    }
    if (Match({ Token::Type::kNil })) {
        Next();
        return std::make_unique<ExpressionObject>(Object());
    }

    if (Match({ Token::Type::kNumber, Token::Type::kString })) {
        assert(Peek()->mObject.has_value());
        std::unique_ptr<IExpression> object = std::make_unique<ExpressionObject>(Peek()->mObject.value());
        Next();
        return object;
    }

    if (Match({ Token::Type::kLeftParen })) {
        Next();
        std::unique_ptr<IExpression> expression = Expression();

        if (!Match({ Token::Type::kRightParen })) {
            throw ParserException("Expected ')' bracket");
        }
        Next();
        return std::make_unique<ExpressionGrouping>(std::move(expression));
    }

    throw ParserException("Unknown token encountered by parser");
}

bool Parser::Match(std::initializer_list<Token::Type> list)
{
    if (mIterator == mTokens.end())
        return false;

    for (auto& type : list) {
        if (mIterator->mType == type)
            return true;
    }
    return false;
}

Token* Parser::Next()
{
    if (mIterator == mTokens.end())
        return nullptr;

    mIterator++;
    if (mIterator != mTokens.end())
        return std::to_address(mIterator);
    return nullptr;
}

Token* Parser::Peek()
{
    if (mIterator == mTokens.end())
        return nullptr;
    return std::to_address(mIterator);
}

}
