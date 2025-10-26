#include "scanner.h"
#include "token.h"

#include <cctype>
#include <ir/ierror_reporter.h>
#include <ir/ir.h>
#include <string>

namespace compiler {

Scanner::Scanner(std::string_view source, ir::IErrorReporter* errorReporter)
    : mSource { source }
    , mErrorReporter { errorReporter }
{
}

Token Scanner::PeekToken()
{
    if (mPeekedToken != std::nullopt) {
        return mPeekedToken.value();
    }
    mPeekedToken = ScanToken();
    return mPeekedToken.value();
}

Token Scanner::ScanToken()
{
    if (mPeekedToken != std::nullopt) {
        Token token { mPeekedToken.value() };
        mPeekedToken.reset();
        return token;
    }

    for (;;) {
        mLexemeStart = mCurrentCharacter;
        if (IsAtEnd()) {
            return MakeToken(Token::Type::kEof);
        }

        char character = Next();
        switch (character) {
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            ++mLine;
            break;

        case '(':
            return MakeToken(Token::Type::kLeftParen);
        case ')':
            return MakeToken(Token::Type::kRightParen);
        case '{':
            return MakeToken(Token::Type::kLeftBrace);
        case '}':
            return MakeToken(Token::Type::kRightBrace);
        case ',':
            return MakeToken(Token::Type::kComma);
        case '.':
            return MakeToken(Token::Type::kDot);
        case '-':
            return MakeToken(Token::Type::kMinus);
        case '+':
            return MakeToken(Token::Type::kPlus);
        case ';':
            return MakeToken(Token::Type::kSemicolon);
        case '*':
            return MakeToken(Token::Type::kStar);

        case '!':
            return MakeToken(Match('=') ? Token::Type::kBangEqual : Token::Type::kBang);
        case '=':
            return MakeToken(Match('=') ? Token::Type::kEqualEqual : Token::Type::kEqual);
        case '<':
            return MakeToken(Match('=') ? Token::Type::kLessEqual : Token::Type::kLess);
        case '>':
            return MakeToken(Match('=') ? Token::Type::kGreaterEqual : Token::Type::kGreater);

        case '/':
            if (Match('/')) {
                while (!IsAtEnd() && Peek() != '\n')
                    Next();
            } else {
                return MakeToken(Token::Type::kSlash);
            }

        case '"':
            return ScanString();

        default:
            if (std::isdigit(character)) {
                return ScanNumber();
            } else if (std::isalpha(character) || character == '_') {
                return ScanIdentifier();
            }
            mErrorReporter->Report(mLine, "[Scanner] Unexpected character.");
            return MakeToken(Token::Type::kError);
        }
    }
}

Token Scanner::ScanIdentifier()
{
    while (std::isalpha(Peek()) || Peek() == '_' || std::isdigit(Peek()))
        Next();

    Token::Type type = Token::Type::kIdentifier;

    std::string identifier = mSource.substr(mLexemeStart,
        mCurrentCharacter - mLexemeStart);

    if (Token::kKeywordToType.find(identifier) != Token::kKeywordToType.end())
        type = Token::kKeywordToType.at(identifier);

    return MakeToken(type);
}

Token Scanner::ScanString()
{
    while (!IsAtEnd() && Peek() != '"') {
        if (Peek() == '\n')
            mLine++;
        Next();
    }

    if (IsAtEnd()) {
        mErrorReporter->Report(mLine, "[Scanner] Unterminated string literal.");
        return MakeToken(Token::Type::kError);
    }

    Next();

    return MakeToken(Token::Type::kString);
}

Token Scanner::ScanNumber()
{
    while (std::isdigit(Peek()))
        Next();

    if (Peek() == '.' && std::isdigit(PeekNext())) {
        Next();

        while (std::isdigit(Peek()))
            Next();
    }

    return MakeToken(Token::Type::kNumber);
}

Token Scanner::MakeToken(Token::Type type)
{
    return Token(type, std::string_view(mSource).substr(mLexemeStart, mCurrentCharacter - mLexemeStart), mLine);
}

bool Scanner::Match(char expected)
{
    if (IsAtEnd())
        return false;
    if (Peek() == expected) {
        Next();
        return true;
    }
    return false;
}

char Scanner::Peek()
{
    if (IsAtEnd())
        return '\0';
    return mSource[mCurrentCharacter];
}

char Scanner::PeekNext()
{
    if (mCurrentCharacter + 1 >= mSource.size())
        return '\0';
    return mSource[mCurrentCharacter + 1];
}

char Scanner::Next()
{
    if (IsAtEnd())
        return '\0';
    return mSource[mCurrentCharacter++];
}

bool Scanner::IsAtEnd()
{
    return mCurrentCharacter >= mSource.size();
}

}
