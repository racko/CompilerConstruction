cc_library(
    name = "json",
    srcs = [
        "json.cpp",
        "jsonLexer.cpp",
        "json.h",
        "jsonLexer.h",
    ],
    deps = [
        "//examples/json/common",
        "//examples/json/lr",
        "//:util",
    ],
)

cc_binary(
    name = "json.exe",
    srcs = ["jsonMain.cpp"],
    deps = [
        ":json",
        "//examples/json/ll",
        "@boost//:filesystem",
    ],
    linkopts = ["-ljsoncpp"],
)

cc_test(
    name = "json_test.exe",
    srcs = ["json_test.cpp"],
    deps = [
        "@catch2",
        ":json",
        "//examples/json/ll",
        "//examples/json/lr",
        "//examples/json/lr2",
        "//:catch_main",
    ],
)
