cc_library(
    name = "functional",
    srcs = [
        "Functional.cpp",
        "FunctionalGrammar.cpp",
        "FunctionalLexer.cpp",
        "FunctionalAttribute.cpp",
        "FunctionalStrategy.cpp",
        "FunctionalEvaluator1.cpp",
        "FunctionalEvaluator2.cpp",
        "FunctionalEnvironment2.cpp",
        "FunctionalValue1.cpp",
        "FunctionalParser.cpp",
        "TokenAttributeConversion.cpp",
        "FunctionalGrammar.h",
        "FunctionalParser.h",
        "FunctionalStrategy.h",
        "FunctionalEnvironment2.h",
        "FunctionalEvaluator1.h",
        "FunctionalAttribute.h",
        "FunctionalValue1.h",
        "TokenAttributeConversion.h",
        "FunctionalLexer.h",
        "FunctionalEvaluator2.h",
        "Functional.h",
    ],
    deps = [
        "//:util",
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
        "//:catch_main",
        "@catch2",
    ],
)

cc_binary(
    name = "functional_fitnesse.exe",
    srcs = ["FitnesseFixture.cpp", "FitnesseMain.c"],
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
