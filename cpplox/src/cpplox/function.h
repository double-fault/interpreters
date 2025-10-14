#pragma once

#include "ast.h"
#include "environment.h"
#include "icallable.h"

#include <memory>

namespace cpplox {

class Function final : public ICallable {
public:
    Function(const std::string& name, std::shared_ptr<Environment> closure,
        std::vector<std::unique_ptr<Token>> parameters, const int arity,
        std::vector<std::unique_ptr<IStatement>> body);

    Object Call(Interpreter* interpreter, std::vector<Object> arguments) override;
    int Arity() override;
    std::string ToString() override;

private:
    const std::string mName;
    std::shared_ptr<Environment> mClosure;
    std::vector<std::unique_ptr<Token>> mParameters;
    const int mArity;
    std::vector<std::unique_ptr<IStatement>> mBody;
};

}
