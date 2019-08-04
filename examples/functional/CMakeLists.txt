add_library(functional
    Functional.cpp
    FunctionalGrammar.cpp
    FunctionalLexer.cpp
    FunctionalAttribute.cpp
    FunctionalStrategy.cpp
    FunctionalEvaluator1.cpp
    FunctionalEvaluator2.cpp
    FunctionalEnvironment2.cpp
    FunctionalValue1.cpp
    FunctionalParser.cpp
    TokenAttributeConversion.cpp
)

target_link_libraries(functional
    PUBLIC util
)

add_executable(functional.exe
    FunctionalMain.cpp
)

target_link_libraries(functional.exe
  PRIVATE
    functional
)

if(BUILD_TESTING)
    add_executable(functional_test.exe
        functional_test.cpp
    )

    target_link_libraries(functional_test.exe
      PRIVATE
        catch2
        functional
        test_main
    )

    add_test(functional_test functional_test.exe)

    add_executable(functional_fitnesse.exe
        FitnesseFixture.cpp
        FitnesseMain.c
    )

    target_link_libraries(functional_fitnesse.exe
      PRIVATE
        functional
        fitnesse
    )

    add_test(functional_fitnesse fitnesse-standalone -d /home/racko/tmp -v -c "Regex.RegexSuite.FunctionalTest?test&format=text")
endif()
