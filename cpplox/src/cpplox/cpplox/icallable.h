#pragma once

#include <memory>
#include <string>
#include <vector>

namespace cpplox {

struct Object;
class Interpreter;
class Environment;
class Instance;

class ICallable {
public:
    virtual Object Call(Interpreter* interpreter, std::vector<Object> arguments) = 0;
    virtual int Arity() const = 0;
    virtual std::string ToString() const = 0;
    virtual void Capture() = 0;
    virtual void Release() = 0;
    virtual Object Bind(std::shared_ptr<Instance> instance) = 0;
};

}
