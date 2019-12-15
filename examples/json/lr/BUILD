cc_library(
    name = "lr",
    srcs = [
        "src/jsonLRGrammar.cpp",
        "src/jsonLRParser.cpp",
        "include/jsonLRGrammar.h",
        "include/jsonLRParser.h",
    ],
    includes = ["include"],
    deps = [
        "//examples/json/common",
        "//Regex:util",
    ],
    visibility = [
        "//examples/json:__subpackages__",
    ],
)
