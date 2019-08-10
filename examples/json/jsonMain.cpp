#include "json.h"                                          // for JsonParser
#include "jsonLLParser.h"                                  // for Parser
#include "json_token.h"                                    // for operator<<
#include <boost/filesystem/operations.hpp>                 // for file_size
#include <boost/filesystem/path_traits.hpp>                // for filesystem
#include <boost/interprocess/detail/os_file_functions.hpp> // for copy_on_w...
#include <boost/interprocess/file_mapping.hpp>             // for file_mapping
#include <boost/interprocess/mapped_region.hpp>            // for mapped_re...
#include <chrono>                                          // for duration
#include <cstdio>                                          // for EOF
#include <fstream>
#include <iostream>      // for operator<<
#include <json/reader.h> // for operator>>
#include <json/value.h>  // for Value
#include <memory>        // for make_unique
#include <ratio>         // for milli
#include <stdexcept>     // for runtime_e...
#include <unordered_set> // for unordered...
#include <vector>        // for vector

namespace json {
struct value;
}

int main(int argc, char** argv) {
    using std::chrono::high_resolution_clock;
    if (argc < 2)
        throw std::runtime_error("Need one argument.");
    auto file_name = argv[1];
    {
        json::JsonParser parser{std::make_unique<jsonLL::Parser>()};
        //{
        //    std::ofstream dfa_file("json_dfa.dot");
        //    dfa_file << parser.lex->dfa;
        //}
        std::cout << std::boolalpha;
        // std::cout << "> ";
        // cin.get(lex.c, 4096);
        // auto ll = cin.gcount();
        namespace bi = boost::interprocess;
        namespace bf = boost::filesystem;
        using bf::file_size;
        using bi::copy_on_write;
        using bi::file_mapping;
        using bi::mapped_region;
        using bi::read_only;
        auto size = file_size(file_name);
        file_mapping file(file_name, read_only);
        mapped_region region(file, copy_on_write, 0, size + 1);
        auto ll = region.get_size();
        std::cout << "read " << ll << " characters" << std::endl;
        // std::cout << "parsing \"" << lex.c << "\"" << std::endl;
        static_cast<char*>(region.get_address())[ll - 1] = EOF;
        region.flush(ll - 1);
        auto t1 = high_resolution_clock::now();
        json::value* result;
        for (auto i = 0; i < 100; i++) {
            result = parser.run(static_cast<char*>(region.get_address()));
        }
        auto t2 = high_resolution_clock::now();
        std::cout << "time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(t2 - t1).count()
                  << "ms" << std::endl;
        // std::cout << *result << std::endl;
    }
    std::ifstream file(file_name);
    auto t1 = high_resolution_clock::now();
    Json::Value v;
    file >> v;
    auto t2 = high_resolution_clock::now();
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(t2 - t1).count()
              << "ms" << std::endl;
    std::cout << "v.size(): " << v.size() << std::endl;

    return 0;
}
