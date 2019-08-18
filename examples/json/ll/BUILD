cc_library(
    name = "ll",
    srcs = [
        "src/jsonLLGrammar.cpp",
        "src/jsonLLParser.cpp",
        "include/jsonLLGrammar.h",
        "include/jsonLLParser.h",
    ],
    includes = ["include"],
    deps = [
        "//examples/json/common",
        "//:util",
    ],
    visibility = [
        "//examples/json:__subpackages__",
    ],
)
