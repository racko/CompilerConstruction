cmake_minimum_required(VERSION 2.8)
project( Regex )

find_package(Boost REQUIRED COMPONENTS system filesystem)
find_library(JSONCPP jsoncpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -pedantic -Wextra -Wconversion -Wsign-conversion -Wshadow -march=native -fconstexpr-steps=-1") #-fconcepts")
#set(CMAKE_CXX_FLAGS "-O3 -flto -std=gnu++11 -Wall -pedantic")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Og")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")

include_directories(
    include
)

add_library(util
    src/Print.cpp
    src/BitSet.cpp
    src/HashSet.cpp
    src/Grammar.cpp
    #src/Grammar2.cpp
    src/lrParser.cpp
    src/llParser.cpp
    #src/lrParser2.cpp
    #src/attributed_lrparser.cpp
    src/Regex.cpp
    src/constexpr.cpp
    src/NumRange.cpp
    src/partition.cpp
    src/NFA_constexpr.cpp
    src/DFA_constexpr.cpp
    src/DFA_minimization_constexpr.cpp
    src/NFA_to_DFA_constexpr.cpp
    src/constexpr_jsonLexer.cpp
)

set(LIBS util ${Boost_LIBRARIES} ${JSONCPP})

add_executable(util_test.exe
    #src/dummy_main.cpp
    src/test_main.cpp
    src/BitSet_test.cpp
    src/HashSet_test.cpp
    src/constexpr_test.cpp
)
target_link_libraries(util_test.exe
    util
    /usr/lib/libstdc++.so.6 # necessary because I use a not-installed clang version
)
add_test(util_test util_test.exe)

## uses deprecated code
#add_executable(test.exe
#    examples/test/main.cpp
#)

#add_executable(constexpr.exe
#    src/constexpr.cpp
#)

add_library(functional
    examples/functional/Functional.cpp
    examples/functional/FunctionalGrammar.cpp
    examples/functional/FunctionalLexer.cpp
    examples/functional/FunctionalAttribute.cpp
    examples/functional/FunctionalStrategy.cpp
    examples/functional/FunctionalEvaluator1.cpp
    examples/functional/FunctionalEvaluator2.cpp
    examples/functional/FunctionalEnvironment2.cpp
    examples/functional/FunctionalValue1.cpp
    examples/functional/FunctionalParser.cpp
    examples/functional/TokenAttributeConversion.cpp
)

add_executable(functional.exe
    examples/functional/FunctionalMain.cpp
)
target_link_libraries(functional.exe functional ${LIBS})

add_executable(functional_test.exe
    examples/functional/test.cpp
    examples/functional/test_main.cpp
)
target_link_libraries(functional_test.exe functional ${LIBS})

add_executable(functional_fitnesse.exe
    examples/functional/FitnesseFixture.cpp
    examples/functional/FitnesseMain.c
)
target_link_libraries(functional_fitnesse.exe
    functional
    ${LIBS}
    /home/racko/tmp/cslim/lib/libCSlim.a

)
target_include_directories(functional_fitnesse.exe PRIVATE
    /home/racko/tmp/cslim/include/Com
    /home/racko/tmp/cslim/include/CSlim
    /home/racko/tmp/cslim/include/ExecutorC
)

add_executable(poker.exe
    examples/poker/pokerMain.cpp
)
target_link_libraries(poker.exe ${LIBS})

add_executable(WhileMain.exe
    examples/while/WhileMain.cpp
    examples/while/While.cpp
)
target_link_libraries(WhileMain.exe ${LIBS})

add_library(json
    examples/json/jsonGrammar.cpp
    examples/json/jsonLLGrammar.cpp
    examples/json/json.cpp
    examples/json/jsonLexer.cpp
    examples/json/jsonParser.cpp
    examples/json/jsonLLParser.cpp
    examples/json/jsonValue.cpp
)

add_executable(json.exe
    examples/json/jsonMain.cpp
)

target_link_libraries(json.exe json ${LIBS})

add_executable(json_test.exe
    examples/json/test.cpp
    examples/json/test_main.cpp
)

target_link_libraries(json_test.exe json ${LIBS})

enable_testing()

add_test(json_test json_test.exe)
add_test(functional_test functional_test.exe)
