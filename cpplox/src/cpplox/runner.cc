#include "runner.h"
#include "ast_printer.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"

#include <iostream>
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

    /*
    spdlog::info("Printing AST..");

    AstPrinter printer {};
    printer.Visit(ptr.get());
    std::cout << printer.GetResult() << "\n";
    */

    spdlog::info("Interpreting AST..");

    Interpreter interpreter(std::move(statements)); 
    interpreter.Run();
}

}
