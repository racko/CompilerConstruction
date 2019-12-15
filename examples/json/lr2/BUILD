cc_library(
    name = "lr2",
    srcs = [
        "src/jsonLRGrammar2.cpp",
        "src/jsonLRParser2.cpp",
        "include/jsonLRGrammar2.h",
        "include/jsonLRParser2.h",
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
