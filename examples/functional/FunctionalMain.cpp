#include "Functional.h"
#include <iostream>

int main() {
    std::cout << std::boolalpha;
    std::cout << "> ";
    std::string line(4096, EOF);
    std::cin.getline(line.data(), line.size()-1);
    auto ll = std::cin.gcount();
    std::cout << "read " << ll << " characters" << std::endl;
    std::cout << "parsing \"" << line << "\"" << std::endl;
    // getline appends a \0, which is included in the gcount()
    line[ll-1] = EOF;
    Functional::run(line.data());

    return 0;
}
