#pragma once

#include "value.h"

#include <cstdint>
#include <initializer_list>
#include <memory>
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
    uint8_t AddConstant(const Value& value, std::shared_ptr<Object> object);
    uint8_t AddConstant(double number);
    uint8_t AddConstant(bool boolean);
    // TODO: AddConstant functions for string, objects needed?

    Value GetConstant(int index) const;

    void Print() const;
    std::string ToString() const;
    friend std::ostream& operator<<(std::ostream& out, const Chunk& chunk);

    std::vector<uint8_t> mBytecode;
    std::vector<int> mLines;
    std::vector<Value> mConstants;

private:
    std::vector<std::shared_ptr<Object>> mSavedObjects;
};

}
