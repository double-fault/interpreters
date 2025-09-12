#pragma once

#include "ierror_reporter.h"
#include "scanner.h"

namespace cpplox {

class Runner final {
public:
    Runner(IErrorReporter* errorReporter);
    void Run(std::string_view source);

private:
    IErrorReporter* mErrorReporter;
};

}
