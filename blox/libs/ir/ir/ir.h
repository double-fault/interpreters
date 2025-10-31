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
    kLocalGet,
    kLocalSet,
    kMultiply,
    kNegate,
    kNil,
    kNot,
    kPop,
    kPopn,
    kPrint,
    kReturn,
    kSubtract,
    kTrue,
    kEof
};

}
