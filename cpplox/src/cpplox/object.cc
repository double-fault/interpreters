#include "object.h"
#include "icallable.h"

#include <boost/type_index.hpp>
#include <fmt/format.h>
#include <magic_enum/magic_enum.hpp>
#include <ostream>
#include <variant>

namespace cpplox {

std::string Object::ToString() const
{
    auto visitor {
        [](auto&& data) -> std::string {
            using T = std::decay_t<decltype(data)>;
            if constexpr (std::is_same_v<std::monostate, T>) {
                return fmt::format("{}= Nil", boost::typeindex::type_id<T>().pretty_name());
            } else if constexpr (std::is_same_v<std::shared_ptr<ICallable>, T>) {
                return fmt::format("fun= {}", data->ToString());
            } else {
                return fmt::format("{}= {}", boost::typeindex::type_id<T>().pretty_name(),
                    data);
            }
        }
    };

    return std::visit(visitor, mData);
}

bool Object::IsNil() const
{
    return std::holds_alternative<std::monostate>(mData);
}

std::ostream& operator<<(std::ostream& out, const Object& literal)
{
    out << literal.ToString();

    return out;
}
}
