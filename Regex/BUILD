package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "util",
    srcs = [
        "BitSet.cpp",
        "Grammar.cpp",
        "Grammar2.cpp",
        "HashSet.cpp",
        "NumRange.cpp",
        "Print.cpp",
        "Regex.cpp",
        "attributed_lrparser.cpp",
        "llParser.cpp",
        "lrParser.cpp",
        "lrParser2.cpp",
        #"DFA_constexpr.cpp",
        #"DFA_minimization_constexpr.cpp",
        #"NFA_constexpr.cpp",
        #"NFA_to_DFA_constexpr.cpp",
        #"constexpr.cpp",
        #"constexpr_jsonLexer.cpp",
        #"partition.cpp",
    ],
    hdrs = [
        "BitSet.h",
        "DFA.h",
        "DFA_fwd.h",
        "DFA_minimization.h",
        "Grammar.h",
        "Grammar2.h",
        "HashSet.h",
        "NFA.h",
        "NFA_fwd.h",
        "NFA_to_DFA.h",
        "NumRange.h",
        "Print.h",
        "Regex.h",
        "Token.h",
        "attributed_lrparser.h",
        "llParser.h",
        "lrParser.h",
        "lrParser2.h",
        "nfaBuilder.h",
        "nfaBuilder_fwd.h",
        "parser.h",
        "token_stream.h",
        "variant.h",
        #"BitSet_constexpr.h",
        #"DFA_constexpr.h",
        #"DFA_minimization_constexpr.h",
        #"HashSet_constexpr.h",
        #"NFA_constexpr.h",
        #"NFA_to_DFA_constexpr.h",
        #"Regex_constexpr.h",
        #"constexpr.h",
        #"constexpr_jsonLexer.h",
        #"hash_table.h",
        #"nfaBuilder_constexpr.h",
        #"partition.h",
    ],
    deps = ["//examples/poker:ranges"],
)

cc_library(
    name = "catch_main",
    srcs = ["test_main.cpp"],
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
