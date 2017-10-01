cmake_minimum_required(VERSION 2.8)
project( Regex )

find_package(Boost REQUIRED COMPONENTS system filesystem)
find_library(JSONCPP jsoncpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -pedantic -Wextra -Wconversion -Wsign-conversion -Wshadow -Werror")
#set(CMAKE_CXX_FLAGS "-O3 -flto -std=gnu++11 -Wall -pedantic")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Og")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

include_directories(
    include
)

add_library(util SHARED
    src/Print.cpp
    src/BitSet.cpp
    src/HashSet.cpp
)

set(LIBS util ${Boost_LIBRARIES} ${JSONCPP})

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
    examples/json/json.cpp
    examples/json/jsonLexer.cpp
    examples/json/jsonParser.cpp
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
