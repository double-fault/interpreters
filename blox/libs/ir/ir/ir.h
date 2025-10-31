#pragma once

#include "chunk.h" // IWYU pragma: keep
#include "object.h" // IWYU pragma: keep
#include "value.h" // IWYU pragma: keep

namespace ir {

// NOTE: The jump instructions jump to absolute ips. Switch to relative instead?
// For real pc ofc relative makes sense, but for this is it worth the effort
// cos then you'll also have to add instructions for backward jmp (instead of
// stuffing a signed offset in the bytecode which is even more pain).
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
    kJump,
    kJumpIfFalse,
    kJumpIfTrue,
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
