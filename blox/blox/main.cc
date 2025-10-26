#include <driver/driver.h>

#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <iterator>

std::string readFile(const char* path)
{
    return { std::istreambuf_iterator<char>(std::ifstream(path).rdbuf()),
        std::istreambuf_iterator<char>() };
}

int main(int argc, char** argv)
{
    if (argc > 2) {
        std::cerr << "Usage: blox [script]" << std::endl;
        return 0;
    }

    spdlog::set_level(spdlog::level::debug);
    spdlog::info("blox starting");

    driver::Driver driver {};

    if (argc == 2) {
        driver.Run(readFile(argv[1]));
    } else {
        std::string line;
        std::cout << "> ";
        while (std::getline(std::cin, line)) {
            driver.Run(line);
            std::cout << "> ";
        }
    }
    return 0;
}
