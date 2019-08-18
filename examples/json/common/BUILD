cc_library(
    name = "common",
    srcs = [
        "src/jsonValue.cpp",
        "src/json_token.cpp",
        "include/jsonValue.h",
        "include/json_token.h",
        "include/num_view.h",
    ],
    includes = ["include"],
    deps = ["//:util"],
    visibility = [
        "//examples/json:__subpackages__",
    ],
)
