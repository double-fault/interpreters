#include "token.h"

#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>
#include <ostream>

namespace compiler {

std::string Token::ToString() const
{
    return fmt::format("token (type={}, lexeme={}, line={})",
        magic_enum::enum_name(mType), mLexeme, mLine);
}

std::ostream& operator<<(std::ostream& out, const Token& token)
{
    out << token.ToString();
    return out;
}

}
