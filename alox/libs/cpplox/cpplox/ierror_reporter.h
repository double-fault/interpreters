#pragma once

#include <string_view>

namespace cpplox {

class IErrorReporter {
public:
    virtual void Report(int line, std::string_view message) = 0;
    virtual bool HadErrors() = 0;

    virtual ~IErrorReporter() = default;
};

}
