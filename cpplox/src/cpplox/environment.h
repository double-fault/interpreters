#pragma once

#include "object.h"

#include <map>

namespace cpplox {

class Environment {
public:
    Environment();
    Environment(Environment*);

    void Define(const std::string& name, const Object& object);
    void Assign(const std::string& name, const Object& object);
    Object* Get(const std::string& name);

private:
    std::map<std::string, Object> mVariables;
    Environment* mEnclosingEnvironment { nullptr };
};

}
