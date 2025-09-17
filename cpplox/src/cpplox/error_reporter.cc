#include "error_reporter.h"

#include <iostream>

namespace cpplox {

void ErrorReporter::Report(int line, std::string_view message)
{
    std::cerr << "[Line " << line << "] Error: " << message << std::endl;
    mHadError = true;
}

bool ErrorReporter::HadErrors()
{
    return mHadError;
}

}
