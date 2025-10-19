#pragma once

#include "icallable.h"

#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace cpplox {

struct Object;
class Interpreter;

class Klass final : public ICallable,
                    public std::enable_shared_from_this<Klass> {
public:
    Klass(const std::string& name, std::shared_ptr<Environment> closure,
        std::map<std::string, Object> methods);

    Object Call(Interpreter* interpreter, std::vector<Object> arguments) override;
    int Arity() const override;
    std::string ToString() const override;
    void Capture() override;
    void Release() override;
    Object Bind(std::shared_ptr<Instance> instance) override;
    std::optional<Object> FindMethod(const std::string& name) const;

private:
    const std::string mName;
    std::shared_ptr<Environment> mEnvironmentCapture;
    std::weak_ptr<Environment> mClosure;
    std::map<std::string, Object> mMethods;
};

}
