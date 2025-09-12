#pragma once

#include "ierror_reporter.h"

#include <iostream>

namespace cpplox {

class ErrorReporter final : public IErrorReporter {
public:
    void Report(int line, std::string_view message) override;
    bool HadErrors() override;

private:
    bool mHadError { false };
};

}
