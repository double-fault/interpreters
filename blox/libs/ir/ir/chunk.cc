#include "chunk.h"
#include "ir.h"
#include "value.h"

#include <cassert>
#include <cstdint>
#include <fmt/format.h>
#include <initializer_list>
#include <ostream>

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

uint8_t Chunk::AddConstant(const Value& value, std::shared_ptr<Object> object)
{
    int ret = mConstants.size();
    assert(ret < 256);

    mConstants.emplace_back(value);

    if (object) {
        mSavedObjects.emplace_back(object);
    }
    return ret;
}

uint8_t Chunk::AddConstant(double number)
{
    return AddConstant(Value(number), nullptr);
}

uint8_t Chunk::AddConstant(bool boolean)
{
    return AddConstant(Value(boolean), nullptr);
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
