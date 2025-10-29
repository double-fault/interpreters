#pragma once

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <map>
#include <string>

namespace compiler {

class Token final {
public:
    enum class Type {
        kError = 0,

        // single-character tokens
        kLeftParen,
        kRightParen,
        kLeftBrace,
        kRightBrace,
        kComma,
        kDot,
        kMinus,
        kPlus,
        kSemicolon,
        kSlash,
        kStar,

        // one or two character tokens
        kBang,
        kBangEqual,
        kEqual,
        kEqualEqual,
        kGreater,
        kGreaterEqual,
        kLess,
        kLessEqual,

        // literals
        kIdentifier,
        kString,
        kNumber,

        // keywords
        kAnd,
        kClass,
        kElse,
        kFalse,
        kFun,
        kFor,
        kIf,
        kNil,
        kOr,
        kPrint,
        kReturn,
        kSuper,
        kThis,
        kTrue,
        kVar,
        kWhile,

        kEof
    };

    // TODO: Will a trie be faster?
    inline static const std::map<std::string, Type> kKeywordToType {
        { "and", Type::kAnd },
        { "class", Type::kClass },
        { "else", Type::kElse },
        { "false", Type::kFalse },
        { "for", Type::kFor },
        { "fun", Type::kFun },
        { "if", Type::kIf },
        { "nil", Type::kNil },
        { "or", Type::kOr },
        { "print", Type::kPrint },
        { "return", Type::kReturn },
        { "super", Type::kSuper },
        { "this", Type::kThis },
        { "true", Type::kTrue },
        { "var", Type::kVar },
        { "while", Type::kWhile }
    };

    Token(Type type, const std::string_view lexeme, int line);

    std::string ToString() const;
    friend std::ostream& operator<<(std::ostream& out, const Token& token);

    Type mType;
    std::string_view mLexeme;
    int mLine;
};

}

template <>
struct fmt::formatter<compiler::Token> : ostream_formatter { };
