#pragma once

#include "ast.h"
#include "environment.h"
#include "icallable.h"

#include <memory>
#include <vector>

namespace cpplox {

class Function final : public ICallable,
                       public std::enable_shared_from_this<Function> {
public:
    Function(const std::string& name, std::shared_ptr<Environment> closure,
        const std::vector<std::string>& parameters, const int arity,
        const std::vector<IStatement*>& body);

    Object Call(Interpreter* interpreter, std::vector<Object> arguments) override;
    int Arity() const override;
    std::string ToString() const override;
    void Capture() override;
    void Release() override;
    Object Bind(std::shared_ptr<Instance> instance) override;

private:
    const std::string mName;
    std::shared_ptr<Environment> mEnvironmentCapture;
    std::weak_ptr<Environment> mClosure;
    std::vector<std::string> mParameters;
    const int mArity;
    std::vector<IStatement*> mBody;
};

}
