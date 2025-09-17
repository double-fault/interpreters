#include "scanner.h"

#include <cctype>

namespace cpplox {

// TODO: Replace string substr with string views?

Scanner::Scanner(std::string_view source, IErrorReporter* errorReporter)
    : mSource { source }
    , mErrorReporter { errorReporter }
{
}

std::vector<Token> Scanner::ScanTokens()
{
    while (!IsAtEnd()) {
        mLexemeStart = mCurrentCharacter;
        ScanToken();
    }

    mTokens.emplace_back(Token::Type::kEof, "", mLine, std::nullopt);
    return mTokens;
}

void Scanner::ScanToken()
{
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
        AddToken(Token::Type::kLeftParen);
        break;
    case ')':
        AddToken(Token::Type::kRightParen);
        break;
    case '{':
        AddToken(Token::Type::kLeftBrace);
        break;
    case '}':
        AddToken(Token::Type::kRightBrace);
        break;
    case ',':
        AddToken(Token::Type::kComma);
        break;
    case '.':
        AddToken(Token::Type::kDot);
        break;
    case '-':
        AddToken(Token::Type::kMinus);
        break;
    case '+':
        AddToken(Token::Type::kPlus);
        break;
    case ';':
        AddToken(Token::Type::kSemicolon);
        break;
    case '*':
        AddToken(Token::Type::kStar);
        break;

    case '!':
        AddToken(Match('=') ? Token::Type::kBangEqual : Token::Type::kBang);
        break;
    case '=':
        AddToken(Match('=') ? Token::Type::kEqualEqual : Token::Type::kEqual);
        break;
    case '<':
        AddToken(Match('=') ? Token::Type::kLessEqual : Token::Type::kLess);
        break;
    case '>':
        AddToken(Match('=') ? Token::Type::kGreaterEqual : Token::Type::kGreater);
        break;

    case '/':
        if (Match('/')) {
            while (!IsAtEnd() && Peek() != '\n')
                Next();
        } else {
            AddToken(Token::Type::kSlash);
        }
        break;

    case '"':
        ScanString();
        break;

    default:
        if (std::isdigit(character)) {
            ScanNumber();
        } else if (std::isalpha(character)) {
            ScanIdentifier();
        } else {
            mErrorReporter->Report(mLine, "[Scanner] Unexpected character.");
        }
        break;
    }
}

void Scanner::ScanIdentifier()
{
    while (std::isalpha(Peek()))
        Next();

    Token::Type type = Token::Type::kIdentifier;

    Token::Literal literal;
    std::string identifier = mSource.substr(mLexemeStart,
        mCurrentCharacter - mLexemeStart);
    literal.mIdentifier = identifier;

    if (Token::kKeywordToType.find(identifier) != Token::kKeywordToType.end())
        type = Token::kKeywordToType.at(identifier);

    AddToken(type, literal);
}

void Scanner::ScanString()
{
    while (!IsAtEnd() && Peek() != '"') {
        if (Peek() == '\n')
            mLine++;
        Next();
    }

    if (IsAtEnd()) {
        mErrorReporter->Report(mLine, "[Scanner] Unterminated string literal.");
        return;
    }

    Next();

    Token::Literal literal;
    literal.mString = mSource.substr(mLexemeStart + 1,
        mCurrentCharacter - mLexemeStart - 2);

    AddToken(Token::Type::kString, literal);
}

void Scanner::ScanNumber()
{
    while (std::isdigit(Peek()))
        Next();

    if (Peek() == '.' && std::isdigit(PeekNext())) {
        Next();

        while (std::isdigit(Peek()))
            Next();
    }

    Token::Literal literal;
    literal.mNumber = std::stod(mSource.substr(mLexemeStart + 1,
        mCurrentCharacter - mLexemeStart));

    AddToken(Token::Type::kNumber, literal);
}

void Scanner::AddToken(Token::Type type, std::optional<Token::Literal> literal)
{
    mTokens.emplace_back(type, mSource.substr(mLexemeStart, mCurrentCharacter - mLexemeStart), mLine, literal);
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
