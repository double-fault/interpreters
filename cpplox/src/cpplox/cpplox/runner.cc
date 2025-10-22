#include "runner.h"
#include "interpreter.h"
#include "parser.h"
#include "resolver.h"
#include "scanner.h"

#include <memory>
#include <spdlog/spdlog.h>

namespace cpplox {

Runner::Runner(IErrorReporter* errorReporter)
    : mErrorReporter { errorReporter }
{
}

void Runner::Run(std::string_view source)
{
    Scanner scanner(source, mErrorReporter);

    spdlog::info("Scanning source..");

    std::vector<Token> tokens = scanner.ScanTokens();
    if (tokens.size() == 1)
        return;

    spdlog::info("Parsing tokens..");

    Parser parser(tokens);
    std::vector<std::unique_ptr<IStatement>> statements = parser.Parse();

    std::vector<IStatement*> statementsPointer;
    for (auto& statement : statements)
        statementsPointer.push_back(statement.get());

    Interpreter interpreter(statementsPointer);
    Resolver resolver(&interpreter, statementsPointer);

    spdlog::info("Resolving..");
    resolver.Resolve();

    spdlog::info("Interpreting AST..");
    interpreter.Run();
}

}
