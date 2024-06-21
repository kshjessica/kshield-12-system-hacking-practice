#pragma clang diagnostic ignored "-Wunused-command-line-argument"

#include <cstdio>
#include <iostream>
#include <string>
#include "../cli/cli.hpp"

void init() {
    std::setvbuf(stdin, 0, 2, 0);
    std::setvbuf(stdout, 0, 2, 0);
    std::setvbuf(stderr, 0, 2, 0);
}

int main() {

    init();

    std::string inp;

    while (true) {
        std::cout << "Command > ";
        std::getline(std::cin, inp);

        Result<uint64_t> run_res = CLI::run(inp);
        if (run_res.get_stats() != ResultStats::Ok) {
            std::cout << "Error > ";
            run_res.get_err().report();
            continue;
        }

        if (run_res.get_ok() == -1)
            break;

        std::cout << "Result > " << run_res.get_ok() << std::endl;
    }

    return 0;
}
