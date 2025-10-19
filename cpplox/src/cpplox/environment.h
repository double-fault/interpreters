#pragma once

#include "object.h"

#include <map>
#include <memory>

namespace cpplox {

class Environment final {
public:
    Environment();
    Environment(std::shared_ptr<Environment> enclosingEnvironment);

    ~Environment();

    void Define(const std::string& name, const Object& object);
    void Assign(const std::string& name, const Object& object);
    Object Get(const std::string& name);
    void Release();

    std::shared_ptr<Environment> mEnclosingEnvironment { nullptr };

private:
    std::map<std::string, Object> mVariables;
    bool mReleased;
};

}
