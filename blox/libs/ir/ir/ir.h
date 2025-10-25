#pragma once

#include "chunk.h" // IWYU pragma: keep
#include "object.h" // IWYU pragma: keep
#include "value.h" // IWYU pragma: keep

namespace ir {

enum class Opcode {
    kError = 0,
    kConstant,
    kNegate,
    kNot,
    kAdd,
    kSubtract,
    kMultiply,
    kDivide,
    kReturn,
    kEof
};

}
