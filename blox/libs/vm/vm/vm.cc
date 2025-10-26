#include "vm.h"
#include "ir/value.h"

#include <cassert>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <ir/ierror_reporter.h>
#include <ir/ir.h>
#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>

using namespace ir;

namespace vm {

Vm::Vm(ObjectFunction* main, IErrorReporter* errorReporter)
    : mMain { main }
    , mErrorReporter { errorReporter }
{
}

void Vm::Run()
{
    spdlog::info("running vm..");

    mCallStack.emplace_back(mMain, 0, 0);
    mChunk = &mMain->mChunk;
    mFrame = &mCallStack[0];

    while (mFrame != nullptr) {
        if (!HasMoreBytes()) {
            spdlog::debug("vm ending..");
            // TODO: implement proper logic here
            // Clean up function stuff from value stack?
            break;
        }

        Byte byte = NextByte();
        Opcode opcode = static_cast<Opcode>(byte.mByte);
        switch (opcode) {
        case Opcode::kConstant:
            Constant(byte);
            break;
        case Opcode::kNegate:
            Negate(byte);
            break;
        case Opcode::kAdd:
        case Opcode::kSubtract:
        case Opcode::kMultiply:
        case Opcode::kDivide:
            Binary(byte);
            break;
        default:
            assert(7 > 8);
        }

        if (mValueStack.empty()) {
            spdlog::debug("value stack empty");
        } else {
            spdlog::debug(fmt::format("Top of value stack= {}", mValueStack.back()));
        }
    }
}

void Vm::Constant(Byte byte)
{
    int index = NextByte().mByte;
    Push(mChunk->GetConstant(index));
}

void Vm::Negate(Byte byte)
{
    Value top = mValueStack.back();
    if (CheckType(Value::Type::kNumber, top, byte.mLine)) {
        Pop();
        Push(Value(-top.mAs.number));
    }
}

void Vm::Binary(Byte byte)
{
    Value b { Pop() };
    Value a { Pop() };

    if (!CheckType(Value::Type::kNumber, { a, b }, byte.mLine)) {
        return;
    }

    Opcode opcode = static_cast<Opcode>(byte.mByte);
    switch (opcode) {
    case Opcode::kAdd:
        Push(Value(a.mAs.number + b.mAs.number));
        break;
    case Opcode::kSubtract:
        Push(Value(a.mAs.number - b.mAs.number));
        break;
    case Opcode::kMultiply:
        Push(Value(a.mAs.number * b.mAs.number));
        break;
    case Opcode::kDivide: {
        if (b.mAs.number == 0.0) {
            mErrorReporter->Report(byte.mLine, "divide by zero");
            return;
        }
        Push(Value(a.mAs.number / b.mAs.number));
        break;
    default:
        assert(6 > 7);
    }
    }
}

Vm::Byte Vm::NextByte()
{
    assert(!IsAtEnd());
    return { mChunk->mBytecode[mFrame->mIp], mChunk->mLines[mFrame->mIp++] };
}

Vm::Byte Vm::PeekByte()
{
    assert(!IsAtEnd());
    return { mChunk->mBytecode[mFrame->mIp], mChunk->mLines[mFrame->mIp] };
}

bool Vm::HasMoreBytes()
{
    return mFrame->mIp < mChunk->mBytecode.size();
}

void Vm::Push(Value value)
{
    mValueStack.push_back(value);
    mStackPointer++;
}

Value Vm::Pop()
{
    Value ret { mValueStack[--mStackPointer] };
    mValueStack.pop_back();
    return ret;
}

Value Vm::Peek(int distance)
{
    return mValueStack[mStackPointer - 1 - distance];
}

bool Vm::CheckType(Value::Type type, Value value, int line)
{
    if (value.mType != type) {
        mErrorReporter->Report(line, fmt::format("Expected type {}, got {}", magic_enum::enum_name(type), magic_enum::enum_name(value.mType)));
        return false;
    }
    return true;
}

bool Vm::CheckType(Value::Type type, std::initializer_list<Value> values, int line)
{
    bool ret { true };
    for (auto& value : values) {
        if (!CheckType(type, value, line)) {
            ret = false;
        }
    }
    return ret;
}

}
