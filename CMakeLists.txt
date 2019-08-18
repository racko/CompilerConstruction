cmake_minimum_required(VERSION 2.8)
project(Regex)

include(CTest)

find_package(Boost REQUIRED COMPONENTS filesystem serialization)
find_library(JSONCPP jsoncpp)

if(NOT JSONCPP)
    message(FATAL_ERROR "jsoncpp library not found")
endif()

add_library(jsoncpp SHARED IMPORTED)
set_property(TARGET jsoncpp PROPERTY IMPORTED_LOCATION ${JSONCPP})

add_library(catch2::catch2 INTERFACE IMPORTED)
set_property(TARGET catch2::catch2 PROPERTY INTERFACE_INCLUDE_DIRECTORIES /usr/include/catch2)

# TODO: replace with separate target_include_directories calls as soon as the libraries are cleaned up
include_directories(include)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

add_compile_options(
    -Wall
    -pedantic
    -Wextra
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -march=native
    $<$<CXX_COMPILER_ID:Clang>:-fconstexpr-steps=-1>
    $<$<CXX_COMPILER_ID:GNU>:-fconcepts>
)

add_library(util
    src/BitSet.cpp
    src/BitSet_constexpr.cpp
    src/DFA.cpp
    src/DFA_constexpr.cpp
    src/DFA_fwd.cpp
    src/DFA_minimization.cpp
    src/DFA_minimization_constexpr.cpp
    src/Grammar.cpp
    src/Grammar2.cpp
    src/HashSet.cpp
    src/HashSet_constexpr.cpp
    src/NFA.cpp
    src/NFA_constexpr.cpp
    src/NFA_fwd.cpp
    src/NFA_to_DFA.cpp
    src/NFA_to_DFA_constexpr.cpp
    src/NumRange.cpp
    src/Print.cpp
    src/Regex.cpp
    src/Regex_constexpr.cpp
    src/Token.cpp
    src/attributed_lrparser.cpp
    src/constexpr.cpp
    src/constexpr_jsonLexer.cpp
    src/hash_table.cpp
    src/llParser.cpp
    src/lrParser.cpp
    src/lrParser2.cpp
    src/nfaBuilder.cpp
    src/nfaBuilder_constexpr.cpp
    src/nfaBuilder_fwd.cpp
    src/parser.cpp
    src/partition.cpp
    src/token_stream.cpp
    src/variant.cpp
)

target_link_libraries(util
  PUBLIC
    ranges
)

if(BUILD_TESTING)
    add_library(fitnesse STATIC IMPORTED)
    set_property(TARGET fitnesse PROPERTY IMPORTED_LOCATION
        /home/racko/tmp/cslim/lib/libCSlim.a
    )
    set_property(TARGET fitnesse PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        /home/racko/tmp/cslim/include
    )

    add_library(test_main STATIC
        src/test_main.cpp
    )

    target_link_libraries(test_main
      PRIVATE
        catch2::catch2
    )

    add_executable(util_test.exe
        src/BitSet_test.cpp
        src/HashSet_test.cpp
        src/constexpr_test.cpp
    )

    target_link_libraries(util_test.exe
      PRIVATE
        catch2::catch2
        test_main
        util
    )

    add_test(util_test util_test.exe)
endif()

add_subdirectory(examples)
