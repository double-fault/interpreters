#pragma once

#include "value.h"

#include <cstdint>
#include <initializer_list>
#include <vector>

namespace ir {

enum class Opcode;

// Not serializable currently
class Chunk final {
public:
    Chunk() = default;

    void AddByte(uint8_t byte, int line);
    void AddBytes(std::initializer_list<uint8_t> bytes, int line);
    void AddByte(Opcode, int line);
    void AddBytes(std::initializer_list<Opcode> opcodes, int line);
    uint8_t AddConstant(const Value& value);
    uint8_t AddConstant(double number);
    uint8_t AddConstant(bool boolean);
    uint8_t AddConstant(Object* object) = delete;
    uint8_t AddConstant(ObjectString* string);
    uint8_t AddConstant(ObjectFunction* function);

    Value GetConstant(int index) const;

    void Print() const;
    std::string ToString() const;
    friend std::ostream& operator<<(std::ostream& out, const Chunk& chunk);

    std::vector<uint8_t> mBytecode;
    std::vector<int> mLines;
    std::vector<Value> mConstants;
};

}
