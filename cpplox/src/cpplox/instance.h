#pragma once

#include <map>
#include <memory>

namespace cpplox {

struct Object;
class Klass;

class Instance final : public std::enable_shared_from_this<Instance> {
public:
    Instance(const std::string& name, std::shared_ptr<Klass> klass);

    std::string ToString() const;
    Object Get(const std::string& name);
    void Set(const std::string& name, const Object& object);

private:
    Object Bind(const Object& object);

    const std::string mName;
    std::shared_ptr<Klass> mKlass;
    std::map<std::string, Object> mFields;
};

}
