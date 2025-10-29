#include "compiler.h"
#include "ir/object.h"
#include "token.h"

#include <cassert>
#include <ir/ierror_reporter.h>
#include <ir/ir.h>
#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>
#include <sys/socket.h>

// TODO: There is fuck-all error handling, once again
// Also note that in the vm if there is an error, it continues to execute?
// Even if its in panic mode print statements will print stuff.. F

namespace compiler {

Compiler::Compiler(std::string_view source, ir::IErrorReporter* errorReporter)
    : mScanner(source, errorReporter)
    , mErrorReporter { errorReporter }
    , mParseRules(magic_enum::enum_count<Token::Type>())
    , mMain { std::make_unique<ir::ObjectFunction>("main", ir::ObjectFunction::Type::kMain, 0) }
    , mCurrentFunction { mMain.get() }
    , mCurrentChunk { &mMain->mChunk }
{
    mErrorReporter->SetPrefix("Compiler");

    struct UnboundRule {
        Token::Type mType;
        std::function<void(Compiler*, Precedence)> mPrefix;
        std::function<void(Compiler*, Precedence)> mInfix;
        Precedence mPrecedence;
    };

    using C = Compiler;
    using T = Token::Type;
    using P = Precedence;
    const std::vector<UnboundRule> rules { {
        { T::kAnd, nullptr, nullptr, P::kNone },
        { T::kBang, &C::Unary, nullptr, P::kNone },
        { T::kBangEqual, nullptr, &C::Binary, P::kEquality },
        { T::kClass, nullptr, nullptr, P::kNone },
        { T::kComma, nullptr, nullptr, P::kNone },
        { T::kDot, nullptr, nullptr, P::kNone },
        { T::kElse, nullptr, nullptr, P::kNone },
        { T::kEof, nullptr, nullptr, P::kNone },
        { T::kEqual, nullptr, nullptr, P::kNone },
        { T::kEqualEqual, nullptr, &C::Binary, P::kEquality },
        { T::kError, nullptr, nullptr, P::kNone },
        { T::kFalse, &C::False, nullptr, P::kNone },
        { T::kFor, nullptr, nullptr, P::kNone },
        { T::kFun, nullptr, nullptr, P::kNone },
        { T::kGreater, nullptr, &C::Binary, P::kComparison },
        { T::kGreaterEqual, nullptr, &C::Binary, P::kComparison },
        { T::kIdentifier, &C::Identifier, nullptr, P::kNone },
        { T::kIf, nullptr, nullptr, P::kNone },
        { T::kLeftBrace, nullptr, nullptr, P::kNone },
        { T::kLeftParen, &C::Grouping, nullptr, P::kNone },
        { T::kLess, nullptr, &C::Binary, P::kComparison },
        { T::kLessEqual, nullptr, &C::Binary, P::kComparison },
        { T::kMinus, &C::Unary, &C::Binary, P::kTerm },
        { T::kNil, &C::Nil, nullptr, P::kNone },
        { T::kNumber, &C::Number, nullptr, P::kNone },
        { T::kOr, nullptr, nullptr, P::kNone },
        { T::kPlus, nullptr, &C::Binary, P::kTerm },
        { T::kPrint, nullptr, nullptr, P::kNone },
        { T::kReturn, nullptr, nullptr, P::kNone },
        { T::kRightBrace, nullptr, nullptr, P::kNone },
        { T::kRightParen, nullptr, nullptr, P::kNone },
        { T::kSemicolon, nullptr, nullptr, P::kNone },
        { T::kSlash, nullptr, &C::Binary, P::kFactor },
        { T::kStar, nullptr, &C::Binary, P::kFactor },
        { T::kString, &C::String, nullptr, P::kNone },
        { T::kSuper, nullptr, nullptr, P::kNone },
        { T::kThis, nullptr, nullptr, P::kNone },
        { T::kTrue, &C::True, nullptr, P::kNone },
        { T::kVar, nullptr, nullptr, P::kNone },
        { T::kWhile, nullptr, nullptr, P::kNone },
    } };
    assert(rules.size() == magic_enum::enum_count<Token::Type>());

    auto bindRule { [this](UnboundRule rule) -> ParseRule {
        ParseRule ret;
        if (rule.mPrefix) {
            ret.mPrefix = std::bind(rule.mPrefix, this, std::placeholders::_1);
        } else {
            ret.mPrefix = nullptr;
        }

        if (rule.mInfix) {
            ret.mInfix = std::bind(rule.mInfix, this, std::placeholders::_1);
        } else {
            ret.mInfix = nullptr;
        }
        ret.mPrecedence = rule.mPrecedence;
        return ret;
    } };

    for (auto& rule : rules) {
        mParseRules[static_cast<int>(rule.mType)] = bindRule(rule);
    }
}

std::unique_ptr<ir::ObjectFunction> Compiler::Compile()
{
    spdlog::info("compiling..");

    Token eof = mScanner.PeekToken();
    while (eof.mType != Token::Type::kEof) {
        Declaration();
        eof = mScanner.PeekToken();
    }

    mCurrentChunk->AddByte(ir::Opcode::kEof, eof.mLine);

    return std::move(mMain);
}

void Compiler::ParseWithPrecedence(Precedence minPrecedence)
{
    Token token = mScanner.PeekToken();
    if (token.mType == Token::Type::kEof)
        return;

    ParseRule rule { GetRule(token) };
    if (!rule.mPrefix) {
        mErrorReporter->Report(token.mLine, fmt::format("Expected expression, instead got token {}", token));
        return;
    }

    rule.mPrefix(minPrecedence);

    for (;;) {
        Token infix = mScanner.PeekToken();
        if (infix.mType == Token::Type::kEqual) {
            mErrorReporter->Report(infix.mLine, "Invalid assignment target");
            return;
        }

        ParseRule infixRule { GetRule(infix) };
        if (!infixRule.mInfix || infixRule.mPrecedence <= minPrecedence)
            break;

        infixRule.mInfix(minPrecedence);
    }
}

void Compiler::Declaration()
{
    Token token { mScanner.PeekToken() };
    if (token.mType == Token::Type::kVar) {
        mScanner.ScanToken();
        DeclarationVariable();
    } else {
        Statement();
    }
}

void Compiler::DeclarationVariable()
{
    Token name { mScanner.ScanToken() };

    std::optional<uint8_t> nameOptional { AddIdentifier(name) };
    if (nameOptional == std::nullopt)
        return;
    uint8_t nameIndex { nameOptional.value() };

    Token equals { mScanner.PeekToken() };
    if (equals.mType == Token::Type::kEqual) {
        mScanner.ScanToken();
        Expression();
    } else {
        mCurrentChunk->AddByte(ir::Opcode::kNil, name.mLine);
    }

    if (!Consume(Token::Type::kSemicolon)) {
        return;
    }

    mCurrentChunk->AddByte(ir::Opcode::kGlobalDefine, name.mLine);
    mCurrentChunk->AddByte(nameIndex, name.mLine);
}

void Compiler::Statement()
{
    Token token { mScanner.PeekToken() };
    if (token.mType == Token::Type::kPrint) {
        StatementPrint();
    } else {
        StatementExpression();
    }
}

void Compiler::StatementPrint()
{
    Token token { mScanner.ScanToken() };
    Expression();

    if (!Consume(Token::Type::kSemicolon)) {
        return;
    }
    mCurrentChunk->AddByte(ir::Opcode::kPrint, token.mLine);
}

void Compiler::StatementExpression()
{
    Expression();
    int line { mScanner.PeekToken().mLine };
    if (Consume(Token::Type::kSemicolon)) {
        mCurrentChunk->AddByte(ir::Opcode::kPop, line);
    }
}

void Compiler::Expression()
{
    ParseWithPrecedence(Precedence::kAssignment);
}

void Compiler::Identifier(Precedence minPrecedence)
{
    Token token { mScanner.ScanToken() };

    std::optional<uint8_t> identifierOptional { AddIdentifier(token) };
    if (identifierOptional == std::nullopt)
        return;
    uint8_t identifierIndex { identifierOptional.value() };

    Token equals { mScanner.PeekToken() };
    if (minPrecedence <= Precedence::kAssignment && equals.mType == Token::Type::kEqual) {
        mScanner.ScanToken();
        Expression();
        mCurrentChunk->AddByte(ir::Opcode::kGlobalSet, token.mLine);
    } else {
        mCurrentChunk->AddByte(ir::Opcode::kGlobalGet, token.mLine);
    }
    mCurrentChunk->AddByte(identifierIndex, token.mLine);
}

void Compiler::Number(Precedence minPrecedence)
{
    Token token = mScanner.ScanToken();
    // TODO: replace std::stod with std::from_chars to avoid unnecessary string copy?
    double number = std::stod(std::string(token.mLexeme));
    uint8_t index = mCurrentChunk->AddConstant(number);
    mCurrentChunk->AddBytes({ static_cast<uint8_t>(ir::Opcode::kConstant), index },
        token.mLine);
}

void Compiler::String(Precedence minPrecedence)
{
    Token token { mScanner.ScanToken() };
    std::string_view string { token.mLexeme.substr(1, token.mLexeme.size() - 2) };
    ir::ObjectString* object { new ir::ObjectString(string) };

    uint8_t index { mCurrentChunk->AddConstant(object) };
    mCurrentChunk->AddByte(ir::Opcode::kConstant, token.mLine);
    mCurrentChunk->AddByte(index, token.mLine);
}

void Compiler::Nil(Precedence minPrecedence)
{
    Token token { mScanner.ScanToken() };
    mCurrentChunk->AddByte(ir::Opcode::kNil, token.mLine);
}

void Compiler::True(Precedence minPrecedence)
{
    Token token { mScanner.ScanToken() };
    mCurrentChunk->AddByte(ir::Opcode::kTrue, token.mLine);
}

void Compiler::False(Precedence minPrecedence)
{
    Token token { mScanner.ScanToken() };
    mCurrentChunk->AddByte(ir::Opcode::kFalse, token.mLine);
}

void Compiler::Unary(Precedence minPrecedence)
{
    Token token = mScanner.ScanToken();
    ParseWithPrecedence(GetRule(token).mPrecedence);

    switch (token.mType) {
    case Token::Type::kMinus:
        mCurrentChunk->AddByte(ir::Opcode::kNegate, token.mLine);
        break;
    case Token::Type::kBang:
        mCurrentChunk->AddByte(ir::Opcode::kNot, token.mLine);
        break;
    default:
        assert(6 > 7);
    }
}

void Compiler::Binary(Precedence minPrecedence)
{
    Token token = mScanner.ScanToken();
    ParseWithPrecedence(GetRule(token).mPrecedence);

    switch (token.mType) {
    case Token::Type::kPlus:
        mCurrentChunk->AddByte(ir::Opcode::kAdd, token.mLine);
        break;
    case Token::Type::kMinus:
        mCurrentChunk->AddByte(ir::Opcode::kSubtract, token.mLine);
        break;
    case Token::Type::kStar:
        mCurrentChunk->AddByte(ir::Opcode::kMultiply, token.mLine);
        break;
    case Token::Type::kSlash:
        mCurrentChunk->AddByte(ir::Opcode::kDivide, token.mLine);
        break;
    case Token::Type::kBangEqual:
        mCurrentChunk->AddBytes({ ir::Opcode::kEqual, ir::Opcode::kNot }, token.mLine);
        break;
    case Token::Type::kEqualEqual:
        mCurrentChunk->AddByte(ir::Opcode::kEqual, token.mLine);
        break;
    case Token::Type::kGreater:
        mCurrentChunk->AddByte(ir::Opcode::kGreater, token.mLine);
        break;
    case Token::Type::kLess:
        mCurrentChunk->AddByte(ir::Opcode::kLess, token.mLine);
        break;
    case Token::Type::kGreaterEqual:
        mCurrentChunk->AddBytes({ ir::Opcode::kLess, ir::Opcode::kNot }, token.mLine);
        break;
    case Token::Type::kLessEqual:
        mCurrentChunk->AddBytes({ ir::Opcode::kGreater, ir::Opcode::kNot }, token.mLine);
        break;
    default:
        mErrorReporter->Report(token.mLine, "Internal error");
    }
}

void Compiler::Grouping(Precedence minPrecedence)
{
    Token token = mScanner.ScanToken();
    Expression();

    Token end = mScanner.ScanToken();
    if (end.mType != Token::Type::kRightParen) {
        mErrorReporter->Report(token.mLine, "Expected ')' at end of grouping");
    }
}

void Compiler::Return(Precedence minPrecedence)
{
    Token token = mScanner.ScanToken();
    // TODO
}

std::optional<uint8_t> Compiler::AddIdentifier(Token token)
{
    if (token.mType != Token::Type::kIdentifier) {
        mErrorReporter->Report(token.mLine, "Expected identifer");
        return std::nullopt;
    }

    return mCurrentChunk->AddConstant(ir::Value(token.mLexeme));
}

Compiler::ParseRule Compiler::GetRule(Token token)
{
    return mParseRules[static_cast<int>(token.mType)];
}

bool Compiler::Consume(Token::Type type)
{
    Token token { mScanner.PeekToken() };
    if (token.mType != type) {
        mErrorReporter->Report(token.mLine,
            fmt::format("Expected token of type {}, got {}",
                magic_enum::enum_name(type), magic_enum::enum_name(token.mType)));
        return false;
    }
    mScanner.ScanToken();
    return true;
}

}
