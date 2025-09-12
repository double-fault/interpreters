#include "runner.h"

#include <iostream>

namespace cpplox {

Runner::Runner(IErrorReporter* errorReporter)
    : mErrorReporter { errorReporter }
{
}

void Runner::Run(std::string_view source) 
{
    Scanner scanner(source, mErrorReporter);

    std::vector<Token> tokens = scanner.ScanTokens();
    for (auto &token: tokens) {
        std::cout << token << "\n";
    }
}

}
