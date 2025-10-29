#include "chunk.h"
#include "ir.h"
#include "value.h"

#include <cassert>
#include <cstdint>
#include <fmt/format.h>
#include <initializer_list>
#include <magic_enum/magic_enum.hpp>
#include <ostream>
#include <spdlog/spdlog.h>

namespace ir {

void Chunk::AddByte(uint8_t byte, int line)
{
    mBytecode.emplace_back(byte);
    mLines.emplace_back(line);
}

void Chunk::AddBytes(std::initializer_list<uint8_t> bytes, int line)
{
    for (auto& byte : bytes) {
        AddByte(byte, line);
    }
}

void Chunk::AddByte(Opcode opcode, int line)
{
    AddByte(static_cast<uint8_t>(opcode), line);
}

void Chunk::AddBytes(std::initializer_list<Opcode> opcodes, int line)
{
    for (auto& opcode : opcodes) {
        AddByte(opcode, line);
    }
}

uint8_t Chunk::AddConstant(const Value& value)
{
    int ret = mConstants.size();
    assert(ret < 256);

    mConstants.emplace_back(value);
    return ret;
}

uint8_t Chunk::AddConstant(double number)
{
    return AddConstant(Value(number));
}

uint8_t Chunk::AddConstant(bool boolean)
{
    return AddConstant(Value(boolean));
}

uint8_t Chunk::AddConstant(ObjectString* string)
{
    return AddConstant(Value(string));
}

uint8_t Chunk::AddConstant(ObjectFunction* function)
{
    return AddConstant(Value(function));
}

Value Chunk::GetConstant(int index) const
{
    return mConstants[index];
}

void Chunk::Print() const
{
    std::string toPrint { fmt::format("== {} ==", ToString()) };
    int line { -1 };

    for (int index { 0 }; index < mBytecode.size(); index++) {
        toPrint += "\n";
        std::string lineString { "|" };
        if (mLines[index] != line) {
            line = mLines[index];
            lineString = std::to_string(line);
        }

        toPrint += fmt::format("{:04d} {:>4} ", index, lineString);

        ir::Opcode opcode { static_cast<ir::Opcode>(mBytecode[index]) };
        toPrint += fmt::format("{:<16}", magic_enum::enum_name(opcode));

        switch (opcode) {
        case ir::Opcode::kGlobalDefine:
        case ir::Opcode::kGlobalGet:
        case ir::Opcode::kGlobalSet:
        case ir::Opcode::kConstant: {
            index++;
            int constantIndex { mBytecode[index] };
            toPrint += fmt::format("{:>4} '{}'", constantIndex, GetConstant(constantIndex));
            break;
        }
        default:
            break;
        }
    }
    spdlog::debug(toPrint);
}

std::string Chunk::ToString() const
{
    return fmt::format("chunk (sizes: bytecode={}, constants={})",
        mBytecode.size(), mConstants.size());
}

std::ostream& operator<<(std::ostream& out, const Chunk& chunk)
{
    out << chunk.ToString();
    return out;
}

}
