#include "error_reporter.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <string>

namespace driver {

void ErrorReporter::SetPrefix(const std::string& prefix)
{
    mPrefix = prefix;
}

void ErrorReporter::SetPanic()
{
    mPanic = true;
}

void ErrorReporter::ResetPanic()
{
    mPanic = false;
}

void ErrorReporter::Report(int line, const std::string& message)
{
    if (mPanic) {
        return;
    }
    mHadErrors = true;
    mPanic = true;

    std::string error {};

    if (!mPrefix.empty()) {
        error = fmt::format("[{}]", mPrefix);
    }

    error += fmt::format("[line={}] {}", line, message);
    spdlog::error(error);
}

bool ErrorReporter::HadErrors()
{
    return mHadErrors;
}

}
