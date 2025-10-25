#pragma once

#include "ierror_reporter.h"
#include "object.h"
#include "token.h"

#include <string>
#include <string_view>
#include <vector>

namespace cpplox {

class Scanner final {
public:
    Scanner(std::string_view source, IErrorReporter* errorReporter);
    std::vector<Token> ScanTokens();

private:
    void ScanToken();
    void AddToken(Token::Type type, std::optional<Object> literal = std::nullopt);

    void ScanIdentifier();
    void ScanString();
    void ScanNumber();

    bool Match(char expected);
    char Peek();
    char PeekNext();
    char Next();
    bool IsAtEnd();

    const std::string mSource;
    int mLexemeStart { 0 };
    int mCurrentCharacter { 0 };
    int mLine { 1 };

    std::vector<Token> mTokens {};

    IErrorReporter* mErrorReporter;
};

}
