#include "parser.h"
#include "ast.h"
#include "object.h"
#include "token.h"

#include <initializer_list>
#include <memory>
#include <optional>
#include <spdlog/spdlog.h>
#include <utility>
#include <vector>

namespace cpplox {

Parser::Parser(const std::vector<Token>& tokens)
    : mTokens { tokens }
{
    mTokens.pop_back(); // Get rid of EOF token
}

std::vector<std::unique_ptr<IStatement>> Parser::Parse()
{
    std::vector<std::unique_ptr<IStatement>> statements;
    while (mIterator != mTokens.end()) {
        statements.push_back(Declaration());
    }
    return statements;
}

// TODO: Synchronize
std::unique_ptr<IStatement> Parser::Declaration()
{
    if (Match({ Token::Type::kVar })) {
        Next();
        return DeclarationVariable();
    } else if (Match({ Token::Type::kFun })) {
        Next();
        return DeclarationFunction();
    }
    return Statement();
}

std::unique_ptr<IStatement> Parser::DeclarationFunction()
{
    std::unique_ptr<Token> identifier = std::make_unique<Token>(*Peek());
    Next();

    if (identifier->mType != Token::Type::kIdentifier) {
        throw ParserException("Expected identifier name after keyword fun");
    }

    if (!Match({ Token::Type::kLeftParen })) {
        throw ParserException("Expected left parentheses after function name");
    }
    Next();

    std::vector<std::unique_ptr<Token>> parameters;
    if (Match({ Token::Type::kRightParen })) {
        Next();
    } else {
        for (;;) {
            if (Peek()->mType != Token::Type::kIdentifier) {
                throw ParserException("Function parameters should be identifiers");
            }

            parameters.push_back(std::make_unique<Token>(*Peek()));
            Next();

            if (Match({ Token::Type::kRightParen })) {
                Next();
                break;
            } else if (Match({ Token::Type::kComma })) {
                Next();
            } else {
                throw ParserException("Unknown delimeter in function argument list");
            }
        }
    }

    if (!Match({ Token::Type::kLeftBrace })) {
        throw ParserException("Expected left brace before function body");
    }
    return std::make_unique<StatementFunction>(std::move(identifier), std::move(parameters), Statement());
}

std::unique_ptr<IStatement> Parser::DeclarationVariable()
{
    if (!Match({ Token::Type::kIdentifier })) {
        throw ParserException("Expected variable name");
    }
    std::unique_ptr<Token> name = std::make_unique<Token>(*Peek());
    Next();

    std::optional<std::unique_ptr<IExpression>> initializer { std::nullopt };
    if (Match({ Token::Type::kEqual })) {
        Next();
        initializer = Expression();
    }

    if (!Match({ Token::Type::kSemicolon })) {
        throw ParserException("Expected semicolon after variable declaration");
    }
    Next();
    return std::make_unique<StatementVariable>(std::move(name), std::move(initializer));
}

std::unique_ptr<IStatement> Parser::Statement()
{
    if (Match({ Token::Type::kPrint })) {
        Next();
        std::unique_ptr<IExpression> expression = Expression();
        if (!Match({ Token::Type::kSemicolon })) {
            throw ParserException("Expected semicolon after statement");
        }
        Next();
        return std::make_unique<StatementPrint>(std::move(expression));
    }

    if (Match({ Token::Type::kLeftBrace })) {
        std::vector<std::unique_ptr<IStatement>> block;
        Next();

        while (!Match({ Token::Type::kRightBrace })) {
            if (mIterator == mTokens.end()) {
                throw ParserException("Expected '}' at end of block");
            }

            block.push_back(Declaration());
        }
        Next();

        return std::make_unique<StatementBlock>(std::move(block));
    }

    if (Match({ Token::Type::kReturn })) {
        Next();
        return Return();
    }

    if (Match({ Token::Type::kIf })) {
        Next();
        return If();
    }

    if (Match({ Token::Type::kWhile })) {
        Next();
        return While();
    }

    if (Match({ Token::Type::kFor })) {
        Next();
        return For();
    }

    if (Match({ Token::Type::kSemicolon })) {
        Next();
        return nullptr;
    }

    return ExpressionStatement();
}

std::unique_ptr<IStatement> Parser::ExpressionStatement()
{
    std::unique_ptr<IExpression> expression = Expression();
    if (!Match({ Token::Type::kSemicolon })) {
        throw ParserException("Expected semicolon after statement");
    }
    Next();
    return std::make_unique<StatementExpression>(std::move(expression));
}

std::unique_ptr<IStatement> Parser::If()
{
    if (!Match({ Token::Type::kLeftParen })) {
        throw ParserException("Expected '(' after if");
    }
    Next();

    std::unique_ptr<IExpression> condition = Expression();

    if (!Match({ Token::Type::kRightParen })) {
        throw ParserException("Expected ')' after if condition");
    }
    Next();

    std::unique_ptr<IStatement> thenStatement = Statement();
    std::unique_ptr<IStatement> elseStatement = nullptr;
    if (Match({ Token::Type::kElse })) {
        Next();
        elseStatement = Statement();
    }

    return std::make_unique<StatementIf>(std::move(condition),
        std::move(thenStatement), std::move(elseStatement));
}

std::unique_ptr<IStatement> Parser::While()
{
    if (!Match({ Token::Type::kLeftParen })) {
        throw ParserException("Expected '(' after while");
    }
    Next();

    std::unique_ptr<IExpression> condition = Expression();

    if (!Match({ Token::Type::kRightParen })) {
        throw ParserException("Expected ')' after while condition");
    }
    Next();

    std::unique_ptr<IStatement> body = Statement();

    return std::make_unique<StatementWhile>(std::move(condition), std::move(body));
}

std::unique_ptr<IStatement> Parser::For()
{
    if (!Match({ Token::Type::kLeftParen })) {
        throw ParserException("Expected '(' after for");
    }
    Next();

    std::unique_ptr<IStatement> initializer { nullptr };

    if (Match({ Token::Type::kSemicolon }))
        Next();
    else if (Match({ Token::Type::kVar })) {
        Next();
        initializer = DeclarationVariable();
    } else {
        initializer = ExpressionStatement();
    }

    std::unique_ptr<IExpression> condition { nullptr };
    if (!Match({ Token::Type::kSemicolon })) {
        condition = Expression();
    }

    if (!Match({ Token::Type::kSemicolon })) {
        throw ParserException("Expected ; after for loop condition");
    }
    Next();

    std::unique_ptr<IExpression> action { nullptr };
    if (!Match({ Token::Type::kRightParen })) {
        action = Expression();
    }

    if (!Match({ Token::Type::kRightParen })) {
        throw ParserException("Expected ) at end of for loop");
    }
    Next();

    std::unique_ptr<IStatement> body { Statement() };

    if (action != nullptr) {
        std::vector<std::unique_ptr<IStatement>> arg;
        arg.push_back(std::move(body));
        arg.push_back(std::make_unique<StatementExpression>(std::move(action)));

        body = std::make_unique<StatementBlock>(std::move(arg));
    }

    if (condition == nullptr) {
        condition = std::make_unique<ExpressionObject>(Object(true));
    }

    body = std::make_unique<StatementWhile>(std::move(condition), std::move(body));

    if (initializer != nullptr) {
        std::vector<std::unique_ptr<IStatement>> arg;
        arg.push_back(std::move(initializer));
        arg.push_back(std::move(body));

        body = std::make_unique<StatementBlock>(std::move(arg));
    }
    return body;
}

std::unique_ptr<IStatement> Parser::Return()
{
    if (Match({ Token::Type::kSemicolon })) {
        Next();
        return std::make_unique<StatementReturn>();
    }
    std::unique_ptr<IStatement> ret = std::make_unique<StatementReturn>(Expression());
    if (!Match({ Token::Type::kSemicolon })) {
        throw ParserException("Expected semicolon after retun");
    }
    Next();
    return ret;
}

std::unique_ptr<IExpression> Parser::Expression()
{
    return Assignment();
}

std::unique_ptr<IExpression> Parser::Assignment()
{
    std::unique_ptr<IExpression> expression = LogicalOr();

    if (Match({ Token::Type::kEqual })) {
        Next();

        ExpressionVariable* variable = dynamic_cast<ExpressionVariable*>(expression.get());
        if (variable == nullptr) {
            throw ParserException("Invalid assignment target");
        }

        return std::make_unique<ExpressionAssignment>(std::move(variable->mName), Assignment());
    }
    return expression;
}

std::unique_ptr<IExpression> Parser::LogicalOr()
{
    std::unique_ptr<IExpression> left = LogicalAnd();

    if (Match({ Token::Type::kOr })) {
        std::unique_ptr<Token> op { std::make_unique<Token>(*Peek()) };
        Next();

        std::unique_ptr<IExpression> right = Equality();

        return std::make_unique<ExpressionLogical>(std::move(left), std::move(op), std::move(right));
    }
    return left;
}

std::unique_ptr<IExpression> Parser::LogicalAnd()
{
    std::unique_ptr<IExpression> left = Equality();

    if (Match({ Token::Type::kAnd })) {
        std::unique_ptr<Token> op { std::make_unique<Token>(*Peek()) };
        Next();

        std::unique_ptr<IExpression> right = Equality();

        return std::make_unique<ExpressionLogical>(std::move(left), std::move(op), std::move(right));
    }
    return left;
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
    return Call();
}

std::unique_ptr<IExpression> Parser::Call()
{
    std::unique_ptr<IExpression> expression = Primary();

    while (Match({ Token::Type::kLeftParen })) {
        std::vector<std::unique_ptr<IExpression>> arguments;

        do {
            Next();
            if (Match({ Token::Type::kRightParen }))
                break;

            arguments.push_back(Expression());
        } while (Match({ Token::Type::kComma }));

        if (!Match({ Token::Type::kRightParen })) {
            throw ParserException("Expected ) after arguments");
        }
        Next();

        if (arguments.size() > 255) {
            throw ParserException("Cannot have more than 255 arguments");
        }

        expression = std::make_unique<ExpressionCall>(std::move(expression), std::move(arguments));
    }

    return expression;
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

    if (Match({ Token::Type::kIdentifier })) {
        std::unique_ptr<IExpression> expression = std::make_unique<ExpressionVariable>(
            std::make_unique<Token>(*Peek()));
        Next();
        return expression;
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
