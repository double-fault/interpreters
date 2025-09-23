#include "token.h"

#include <magic_enum/magic_enum.hpp>

#include <iostream>

namespace cpplox {

Token::Token(Token::Type type, const std::string& lexeme, int line,
    const std::optional<Literal> literal)
    : mType { type }
    , mLexeme { lexeme }
    , mLine { line }
    , mLiteral { std::move(literal) }
{
}

std::ostream& operator<<(std::ostream& out, const Token& token)
{
    out << magic_enum::enum_name(token.mType) << ": " << token.mLexeme << " (line=" << token.mLine;

    if (token.mLiteral.has_value()) 
        out << token.mLiteral.value();

    out << ")";
    return out;
}

}
