#pragma once

#include "token.h"

#include <ir/ierror_reporter.h>
#include <ir/ir.h>
#include <string_view>

namespace compiler {

class Scanner final {
public:
    Scanner(std::string_view source, ir::IErrorReporter* errorReporter);
    Token PeekToken();
    Token ScanToken();

private:
    Token MakeToken(Token::Type type);

    Token ScanIdentifier();
    Token ScanString();
    Token ScanNumber();

    bool Match(char expected);
    char Peek();
    char PeekNext();
    char Next();
    bool IsAtEnd();

    int mLexemeStart { 0 };
    int mCurrentCharacter { 0 };
    int mLine { 1 };
    std::optional<Token> mPeekedToken;

    const std::string mSource;
    ir::IErrorReporter* mErrorReporter;
};

}
