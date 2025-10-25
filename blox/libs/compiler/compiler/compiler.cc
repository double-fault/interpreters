#include "compiler.h"
#include "ir/object.h"
#include "token.h"

#include <cassert>
#include <ir/ierror_reporter.h>
#include <ir/ir.h>
#include <magic_enum/magic_enum.hpp>

namespace compiler {

Compiler::Compiler(std::string_view source, ir::IErrorReporter* errorReporter)
    : mScanner(source, errorReporter)
    , mErrorReporter { errorReporter }
    , mParseRules(magic_enum::enum_count<Token::Type>())
    , mMain { std::make_unique<ir::ObjectFunction>("main", ir::ObjectFunction::Type::kMain, 0) }
    , mCurrentFunction { mMain.get() }
    , mCurrentChunk { &mMain->mChunk }
{
    struct UnboundRule {
        Token::Type mType;
        std::function<void(Compiler*, Token)> mPrefix;
        std::function<void(Compiler*, Token)> mInfix;
        Precedence mPrecedence;
    };

    using C = Compiler;
    using T = Token::Type;
    using P = Precedence;
    const std::vector<UnboundRule> rules { {
        { T::kAnd, nullptr, nullptr, P::kNone },
        { T::kBang, nullptr, nullptr, P::kNone },
        { T::kBangEqual, nullptr, nullptr, P::kNone },
        { T::kClass, nullptr, nullptr, P::kNone },
        { T::kComma, nullptr, nullptr, P::kNone },
        { T::kDot, nullptr, nullptr, P::kNone },
        { T::kElse, nullptr, nullptr, P::kNone },
        { T::kEof, nullptr, nullptr, P::kNone },
        { T::kEqual, nullptr, nullptr, P::kNone },
        { T::kEqualEqual, nullptr, nullptr, P::kNone },
        { T::kError, nullptr, nullptr, P::kNone },
        { T::kFalse, nullptr, nullptr, P::kNone },
        { T::kFor, nullptr, nullptr, P::kNone },
        { T::kFun, nullptr, nullptr, P::kNone },
        { T::kGreater, nullptr, nullptr, P::kNone },
        { T::kGreaterEqual, nullptr, nullptr, P::kNone },
        { T::kIdentifier, nullptr, nullptr, P::kNone },
        { T::kIf, nullptr, nullptr, P::kNone },
        { T::kLeftBrace, nullptr, nullptr, P::kNone },
        { T::kLeftParen, &C::Grouping, nullptr, P::kNone },
        { T::kLess, nullptr, nullptr, P::kNone },
        { T::kLessEqual, nullptr, nullptr, P::kNone },
        { T::kMinus, &C::Unary, &C::Binary, P::kTerm },
        { T::kNil, nullptr, nullptr, P::kNone },
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
        { T::kString, nullptr, nullptr, P::kNone },
        { T::kSuper, nullptr, nullptr, P::kNone },
        { T::kThis, nullptr, nullptr, P::kNone },
        { T::kTrue, nullptr, nullptr, P::kNone },
        { T::kVar, nullptr, nullptr, P::kNone },
        { T::kWhile, nullptr, nullptr, P::kNone },
    } };
    assert(rules.size() == magic_enum::enum_count<Token::Type>());

    for (auto& rule : rules) {
        mParseRules[static_cast<int>(rule.mType)] = ParseRule {
            std::bind(rule.mPrefix, this, std::placeholders::_1),
            std::bind(rule.mInfix, this, std::placeholders::_1),
            rule.mPrecedence
        };
    }
}

std::unique_ptr<ir::ObjectFunction> Compiler::Compile()
{
    Expression();

    Token eof = mScanner.ScanToken();
    if (eof.mType != Token::Type::kEof) {
        mErrorReporter->Report(eof.mLine, "internal error, not at EOF after finishing compilation");
    }
    mCurrentChunk->AddByte(ir::Opcode::kEof, eof.mLine);

    return std::move(mMain);
}

void Compiler::ParseWithPrecedence(Precedence minPrecedence)
{
    Token token = mScanner.ScanToken();
    if (token.mType == Token::Type::kEof)
        return;

    ParseRule rule { GetRule(token) };
    if (rule.mPrefix == nullptr) {
        mErrorReporter->Report(token.mLine, "Expected expression");
        return;
    }

    rule.mPrefix(token);

    for (;;) {
        Token infix = mScanner.ScanToken();
        ParseRule infixRule { GetRule(infix) };
        if (infixRule.mInfix == nullptr || infixRule.mPrecedence < minPrecedence)
            break;

        infixRule.mInfix(infix);
    }
}

void Compiler::Expression()
{
    ParseWithPrecedence(Precedence::kAssignment);
}

void Compiler::Number(Token token)
{
    // TODO: replace std::stod with std::from_chars to avoid unnecessary string copy?
    double number = std::stod(std::string(token.mLexeme));
    uint8_t index = mCurrentChunk->AddConstant(number);
    mCurrentChunk->AddBytes({ static_cast<uint8_t>(ir::Opcode::kConstant), index },
        token.mLine);
}

void Compiler::Unary(Token token)
{
    Expression();

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

void Compiler::Binary(Token token)
{
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
    default:
        mErrorReporter->Report(token.mLine, "Internal error");
    }
}

void Compiler::Grouping(Token token)
{
    Expression();

    Token end = mScanner.ScanToken();
    if (end.mType != Token::Type::kRightParen) {
        mErrorReporter->Report(token.mLine, "Expected ')' at end of grouping");
    }
}

Compiler::ParseRule Compiler::GetRule(Token token)
{
    return mParseRules[static_cast<int>(token.mType)];
}

}
