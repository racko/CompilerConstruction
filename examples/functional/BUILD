cc_library(
    name = "functional",
    srcs = [
        "Functional.cpp",
        "Functional.h",
        "FunctionalAttribute.cpp",
        "FunctionalAttribute.h",
        "FunctionalEnvironment2.cpp",
        "FunctionalEnvironment2.h",
        "FunctionalEvaluator1.cpp",
        "FunctionalEvaluator1.h",
        "FunctionalEvaluator2.cpp",
        "FunctionalEvaluator2.h",
        "FunctionalGrammar.cpp",
        "FunctionalGrammar.h",
        "FunctionalLexer.cpp",
        "FunctionalLexer.h",
        "FunctionalParser.cpp",
        "FunctionalParser.h",
        "FunctionalStrategy.cpp",
        "FunctionalStrategy.h",
        "FunctionalValue1.cpp",
        "FunctionalValue1.h",
        "TokenAttributeConversion.cpp",
        "TokenAttributeConversion.h",
    ],
    deps = [
        "//Regex:util",
    ],
)

cc_binary(
    name = "functional.exe",
    srcs = ["FunctionalMain.cpp"],
    deps = [":functional"],
)

cc_test(
    name = "functional_test",
    srcs = ["functional_test.cpp"],
    deps = [
        ":functional",
        "//Regex:catch_main",
        "@catch2",
    ],
)

cc_binary(
    name = "functional_fitnesse.exe",
    srcs = [
        "FitnesseFixture.cpp",
        "FitnesseMain.c",
    ],
    deps = [
        ":functional",
        "@cslim",
    ],
)

sh_test(
    name = "functional_fitnesse_test",
    srcs = ["run_fitnesse_tests.sh"],
    data = [
        "pages/FunctionalTest/content.txt",
        ":functional_fitnesse.exe",
    ],
)
