#pragma once

#include <ir/ierror_reporter.h>
#include <string>

namespace driver {

class ErrorReporter final : public ir::IErrorReporter {
public:
    void SetPrefix(const std::string& prefix) override;
    void SetPanic() override;
    void ResetPanic() override;
    void Report(int line, const std::string& message) override;
    bool HadErrors() override;

private:
    std::string mPrefix;
    bool mPanic { false };
    bool mHadErrors { false };
};

}
