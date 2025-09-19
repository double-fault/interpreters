#pragma once

#include <map>
#include <optional>
#include <string>

namespace cpplox {

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

    struct Literal {
        std::string mIdentifier;
        std::string mString;
        double mNumber;
    };

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

    Token(Type type, const std::string& lexeme, int line, std::optional<Literal> literal);

    friend std::ostream& operator<<(std::ostream& out, const Token& token);

    const Type mType;
    const std::string mLexeme;
    const int mLine;
    std::optional<Literal> mLiteral;
};

}
