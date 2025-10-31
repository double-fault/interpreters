#include "vm.h"
#include "ir/value.h"

#include <cassert>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <iostream>
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
    mErrorReporter->SetPrefix("VM");
}

void Vm::Run()
{
    spdlog::info("running vm..");

    mCallStack.emplace_back(mMain, 0, 0);
    mChunk = &mMain->mChunk;
    mFrame = &mCallStack[0];

    while (mFrame != nullptr) {
        /*
        if (!HasMoreBytes()) {
            // TODO: implement proper logic here
            // Clean up function stuff from value stack?
            break;
        }*/

        Byte byte = NextByte();
        Opcode opcode = static_cast<Opcode>(byte.mByte);
        spdlog::debug("Interpreting opcode {}", magic_enum::enum_name(opcode));
        switch (opcode) {
        case Opcode::kGlobalDefine:
        case Opcode::kGlobalGet:
        case Opcode::kGlobalSet:
            Global(byte);
            break;
        case Opcode::kLocalGet:
        case Opcode::kLocalSet:
            Local(byte);
            break;
        case Opcode::kConstant:
            Constant(byte);
            break;
        case Opcode::kNil:
            Push(Value());
            break;
        case Opcode::kTrue:
            Push(Value(true));
            break;
        case Opcode::kFalse:
            Push(Value(false));
            break;
        case Opcode::kNegate:
            Negate(byte);
            break;
        case Opcode::kNot:
            Push(!IsTrue(Pop()));
            break;
        case Opcode::kAdd:
        case Opcode::kSubtract:
        case Opcode::kMultiply:
        case Opcode::kDivide:
        case Opcode::kEqual:
        case Opcode::kLess:
        case Opcode::kGreater:
            Binary(byte);
            break;
        case Opcode::kPrint:
            Print(byte);
            break;
        case Opcode::kPop:
            Pop();
            break;
        case Opcode::kPopn:
            Popn(byte);
            break;
        case Opcode::kEof:
            return;
        default:
            spdlog::error("Internal error - unknown opcode {}",
                magic_enum::enum_name(opcode));
        }
    }
}

void Vm::Global(Byte byte)
{
    Opcode opcode { static_cast<Opcode>(byte.mByte) };
    uint8_t index { NextByte().mByte };
    ObjectString* name { static_cast<ObjectString*>(mChunk->GetConstant(index).mAs.object) };

    switch (opcode) {
    case Opcode::kGlobalDefine:
        mGlobals[name->mString] = Pop();
        break;
    case Opcode::kGlobalSet:
        mGlobals[name->mString] = mValueStack.back();
        break;
    case Opcode::kGlobalGet:
        if (mGlobals.find(name->mString) == mGlobals.end()) {
            mErrorReporter->Report(byte.mLine, fmt::format("Unknown global {}", name->mString));
        } else {
            Push(mGlobals[name->mString]);
        }
        break;
    default:
        assert(10 > 11);
    }
}

void Vm::Local(Byte byte)
{
    Opcode opcode { static_cast<Opcode>(byte.mByte) };
    uint8_t index { NextByte().mByte };

    switch (opcode) {
    case Opcode::kLocalGet:
        Push(mValueStack[index]);
        break;
    case Opcode::kLocalSet:
        mValueStack[index] = mValueStack.back();
        break;
    default:
        assert(11 > 12);
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
    Opcode opcode = static_cast<Opcode>(byte.mByte);

    if (opcode == Opcode::kAdd && a.mType == ir::Value::Type::kString) {
        if (!CheckType(Value::Type::kString, b, byte.mLine)) {
            return;
        }
        ObjectString* objectA { static_cast<ObjectString*>(a.mAs.object) };
        ObjectString* objectB { static_cast<ObjectString*>(b.mAs.object) };
        Push(Value(objectA->mString + objectB->mString));
        return;
    }

    if (opcode == Opcode::kEqual) {
        Push(Value(a == b));
        return;
    }

    if (!CheckType(Value::Type::kNumber, { a, b }, byte.mLine)) {
        return;
    }

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
    }
    case Opcode::kGreater:
        Push(Value(a.mAs.number > b.mAs.number));
        break;
    case Opcode::kLess:
        Push(Value(a.mAs.number < b.mAs.number));
        break;
    default:
        assert(6 > 7);
    }
}

void Vm::Print(Byte byte)
{
    // TODO: Clean this up, nothing should be run if errors are there
    if (mErrorReporter->HadErrors()) {
        return;
    }
    std::cout << Pop() << "\n";
}

void Vm::Popn(Byte byte)
{
    uint8_t count { NextByte().mByte };
    for (uint8_t i = 0; i < count; i++) {
        Pop();
    }
}

Vm::Byte Vm::NextByte()
{
    assert(HasMoreBytes());
    return { mChunk->mBytecode[mFrame->mIp], mChunk->mLines[mFrame->mIp++] };
}

Vm::Byte Vm::PeekByte()
{
    assert(HasMoreBytes());
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

bool Vm::IsTrue(Value value)
{
    if (value.mType == Value::Type::kNil || (value.mType == Value::Type::kBool && value.mAs.boolean == false)) {
        return false;
    }
    return true;
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
