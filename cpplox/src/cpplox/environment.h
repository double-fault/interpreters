#pragma once

#include "object.h"

#include <map>
#include <memory>

namespace cpplox {

class Environment {
public:
    Environment();
    Environment(std::unique_ptr<Environment>);

    void Define(const std::string& name, const Object& object);
    void Assign(const std::string& name, const Object& object);
    Object* Get(const std::string& name);

    std::unique_ptr<Environment> mEnclosingEnvironment { nullptr };

private:
    std::map<std::string, Object> mVariables;
};

}
