#include "json.h"
#include "jsonLexer.h"
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <json/json.h>

int main(int argc, char** argv) {
    using std::chrono::high_resolution_clock;
    if (argc < 2)
        throw std::runtime_error("Need one argument.");
    auto file_name = argv[1];
    {
        JsonParser parser;
        {
            std::ofstream dfa_file("json_dfa.dot");
            dfa_file << parser.lex->dfa;
        }
        std::cout << std::boolalpha;
        //cout << "> ";
        //cin.get(lex.c, 4096);
        //auto ll = cin.gcount();
        namespace bi = boost::interprocess;
        namespace bf = boost::filesystem;
        using bi::file_mapping;
        using bi::mapped_region;
        using bi::read_only;
        using bi::copy_on_write;
        using bf::file_size;
        auto size = file_size(file_name);
        file_mapping file(file_name, read_only);
        mapped_region region(file, copy_on_write, 0, size + 1);
        auto ll = region.get_size();
        cout << "read " << ll << " characters" << endl;
        //std::cout << "parsing \"" << lex.c << "\"" << std::endl;
        static_cast<char*>(region.get_address())[ll-1] = EOF;
        region.flush(ll-1);
        auto t1 = high_resolution_clock::now();
        value* result;
        for (auto i = 0; i < 100; i++) {
            result = parser.run(static_cast<char*>(region.get_address()));
        }
        auto t2 = high_resolution_clock::now();
        std::cout << "time: " << std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(t2 - t1).count() << "ms" << std::endl;
        //std::cout << *result << std::endl;
    }
    std::ifstream file(file_name);
    auto t1 = high_resolution_clock::now();
    Json::Value v;
    file >> v;
    auto t2 = high_resolution_clock::now();
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::duration<double,std::milli>>(t2 - t1).count() << "ms" << std::endl;
    std::cout << "v.size(): " << v.size() << std::endl;

    return 0;
}
