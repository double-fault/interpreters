#pragma once

#include "chunk.h"

#include <string>

namespace ir {

// Object or its derived classes are not necessarily copyable
class Object {
public:
    virtual std::string ToString() const = 0;
    friend std::ostream& operator<<(std::ostream& out, const Object& object);
    virtual ~Object() = default;
};

class ObjectString final : public Object {
public:
    ObjectString(std::string_view string);
    std::string ToString() const override;

    const std::string mString;
};

class ObjectFunction final : public Object {
public:
    enum class Type {
        kError = 0,
        kMain,
        kFunction
    };

    ObjectFunction(const std::string& name, Type type, const int arity);
    std::string ToString() const override;

    const std::string mName;
    Type mType;
    const int mArity;
    Chunk mChunk;
};

}
