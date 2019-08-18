package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "util",
    srcs = [
        "src/NumRange.cpp",
        "src/BitSet.cpp",
        #"src/DFA_constexpr.cpp",
        #"src/DFA_minimization_constexpr.cpp",
        "src/Grammar.cpp",
        "src/Grammar2.cpp",
        "src/HashSet.cpp",
        #"src/NFA_constexpr.cpp",
        #"src/NFA_to_DFA_constexpr.cpp",
        "src/Print.cpp",
        "src/Regex.cpp",
        "src/attributed_lrparser.cpp",
        #"src/constexpr.cpp",
        #"src/constexpr_jsonLexer.cpp",
        "src/llParser.cpp",
        "src/lrParser.cpp",
        "src/lrParser2.cpp",
        #"src/partition.cpp",
    ],
    hdrs = [
        "include/NumRange.h",
        "include/BitSet.h",
        #"include/BitSet_constexpr.h",
        "include/DFA.h",
        "include/DFA_fwd.h",
        #"include/DFA_constexpr.h",
        "include/DFA_minimization.h",
        #"include/DFA_minimization_constexpr.h",
        "include/Grammar.h",
        "include/Grammar2.h",
        "include/HashSet.h",
        #"include/HashSet_constexpr.h",
        "include/NFA.h",
        #"include/NFA_constexpr.h",
        "include/NFA_fwd.h",
        "include/NFA_to_DFA.h",
        #"include/NFA_to_DFA_constexpr.h",
        "include/Print.h",
        "include/Regex.h",
        #"include/Regex_constexpr.h",
        "include/Token.h",
        "include/attributed_lrparser.h",
        #"include/constexpr.h",
        #"include/constexpr_jsonLexer.h",
        #"include/hash_table.h",
        "include/llParser.h",
        "include/lrParser.h",
        "include/lrParser2.h",
        "include/nfaBuilder.h",
        #"include/nfaBuilder_constexpr.h",
        "include/nfaBuilder_fwd.h",
        "include/parser.h",
        #"include/partition.h",
        "include/token_stream.h",
        "include/variant.h",
    ],
    includes = [
        "include",
    ],
)

cc_library(
    name = "catch_main",
    srcs = ["src/test_main.cpp"],
    linkstatic = True,
    deps = ["@catch2"],
)

load("@bazel_compilation_database//:aspects.bzl", "compilation_database")

compilation_database(
    name = "compdb",
    # ideally should be the same as `bazel info execution_root`.
    exec_root = "/tmp/nvme/racko/bazel_cache/_bazel_racko/a01f0cba51a77ac3093e4226478c33ae/execroot/__main__",
    targets = [
        #"//examples/functional",
        #"//examples/functional:functional.exe",
        #"//examples/functional:functional_test",
        #"//examples/functional:functional_fitnesse.exe",
    ],
)
