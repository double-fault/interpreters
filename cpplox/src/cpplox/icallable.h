#pragma once

#include <string>
#include <vector>

namespace cpplox {

class Interpreter;
struct Object;

class ICallable {
public:
    virtual Object Call(Interpreter* interpreter, std::vector<Object> arguments) = 0;
    virtual int Arity() = 0;
    virtual std::string ToString() = 0;
};

}
