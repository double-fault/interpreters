#pragma once

#include <string>

namespace ir {

class IErrorReporter {
public:
    virtual void SetPrefix(const std::string& prefix) = 0;
    /*
     * Report() should start panic mode
     * While in panic, no further errors should be reported
     */
    virtual void SetPanic() = 0;
    virtual void ResetPanic() = 0;
    virtual void Report(int line, const std::string& message) = 0;
    virtual bool HadErrors() = 0;

    virtual ~IErrorReporter() = default;
};

}
