#pragma once

#include "ierror_reporter.h"

namespace cpplox {

class Runner final {
public:
    Runner(IErrorReporter* errorReporter);
    void Run(std::string_view source);

private:
    IErrorReporter* mErrorReporter;
};

}
