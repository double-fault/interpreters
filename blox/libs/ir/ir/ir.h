#pragma once

#include "chunk.h" // IWYU pragma: keep
#include "object.h" // IWYU pragma: keep
#include "value.h" // IWYU pragma: keep

namespace ir {

enum class Opcode {
    kError = 0,
    kAdd,
    kConstant,
    kDivide,
    kEqual,
    kFalse,
    kGlobalDefine,
    kGlobalGet,
    kGlobalSet,
    kGreater,
    kLess,
    kMultiply,
    kNegate,
    kNil,
    kNot,
    kPop,
    kPrint,
    kReturn,
    kSubtract,
    kTrue,
    kEof
};

}
