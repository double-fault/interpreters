#pragma once

#include <string_view>

namespace driver {

class Driver final {
public:
    Driver() = default;
    bool Run(std::string_view source); // returns false if there was any error
};

}
