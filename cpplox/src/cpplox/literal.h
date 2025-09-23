#pragma once

#include <string>

namespace cpplox {

struct Literal {
    enum class Type {
        kError = 0,

        kIdentifier,
        kString,
        kNumber
    };

    std::string ToString() const;

    friend std::ostream& operator<<(std::ostream& out, const Literal& token);

    Type mType;
    std::string mIdentifier;
    std::string mString;
    double mNumber;
};

};
