cc_library(
    name = "json",
    srcs = [
        "json.cpp",
        "json.h",
        "jsonLexer.cpp",
        "jsonLexer.h",
    ],
    deps = [
        "//Regex:util",
        "//examples/json/common",
        "//examples/json/lr",
    ],
)

cc_binary(
    name = "json.exe",
    srcs = ["jsonMain.cpp"],
    linkopts = ["-ljsoncpp"],
    deps = [
        ":json",
        "//examples/json/ll",
        "@boost//:filesystem",
    ],
)

cc_test(
    name = "json_test.exe",
    srcs = ["json_test.cpp"],
    deps = [
        ":json",
        "//Regex:catch_main",
        "//examples/json/ll",
        "//examples/json/lr",
        "//examples/json/lr2",
        "@catch2",
    ],
)
