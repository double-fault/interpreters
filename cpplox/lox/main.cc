#include <runner.h>
#include <error_reporter.h>

#include <iostream>
#include <memory>
#include <fstream>
#include <iterator>

std::string readFile(const char* path) 
{
    return { std::istreambuf_iterator<char>(std::ifstream(path).rdbuf()),
         std::istreambuf_iterator<char>() };
}

int main(int argc, char** argv) {
    if (argc > 2) {
        std::cerr << "Usage: lox [script]" << std::endl;
        return 0;
    } 

    std::unique_ptr<cpplox::IErrorReporter> errorReporter = 
        std::make_unique<cpplox::ErrorReporter>();
    cpplox::Runner runner(errorReporter.get());

    if (argc == 2) {
        runner.Run(readFile(argv[1]));
    } else {
        std::string line;
        std::cout << "> ";
        while (std::getline(std::cin, line)) {
            runner.Run(line);
            std::cout << "> ";
        }
    }
    return 0;
}
