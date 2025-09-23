#include "token.h"
#include "object.h"

#include <magic_enum/magic_enum.hpp>

#include <iostream>

namespace cpplox {

Token::Token(Token::Type type, const std::string& lexeme, int line,
    const std::optional<Object> object)
    : mType { type }
    , mLexeme { lexeme }
    , mLine { line }
    , mObject { std::move(object) }
{
}

std::ostream& operator<<(std::ostream& out, const Token& token)
{
    out << magic_enum::enum_name(token.mType) << ": " << token.mLexeme << " (line=" << token.mLine;

    if (token.mObject.has_value()) 
        out << token.mObject.value();

    out << ")";
    return out;
}

}
