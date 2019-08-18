load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_compilation_database",
    sha256 = "bb1b812396e2ee36a50a13b03ae6833173ce643e8a4bd50731067d0b4e5c6e86",
    strip_prefix = "bazel-compilation-database-0.3.5",
    urls = ["https://github.com/grailbio/bazel-compilation-database/archive/0.3.5.tar.gz"],
)

new_local_repository(
    name = "catch2",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "catch2",
    hdrs = glob(["**/*.hpp"]),
    includes = ["./",],
)
""",
    path = "/usr/include/catch2",
)

# I exclude headers because I can't check whether they are used from external or from /usr/include anyhow.
new_local_repository(
    name = "boost",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
#cc_library(
#    name = "iterator",
#    hdrs = glob([
#        "include/boost/iterator_adaptors.hpp",
#        "include/boost/iterator_adaptors.hpp",
#        "include/boost/iterator_adaptors.hpp",
#        "include/boost/iterator_adaptors.hpp",
#        "include/boost/iterator/**/*.hpp",
#        "include/boost/pending/**/*.hpp",
#    ]),
#    strip_include_prefix = "include",
#)
cc_library(
    name = "system",
    srcs = ["lib/libboost_system.so"],
    #hdrs = glob([
    #    "include/boost/system/**/*.hpp",
    #    "include/boost/cerrno.hpp",
    #]),
    #strip_include_prefix = "include",
)
cc_library(
    name = "filesystem",
    srcs = ["lib/libboost_filesystem.so",],
    #hdrs = glob([
    #    "include/boost/filesystem/**/*.hpp",
    #    "include/boost/filesystem.hpp",
    #]),
    #strip_include_prefix = "include",
)
cc_library(
    name = "serialization",
    srcs = ["lib/libboost_serialization.so"],
    #hdrs = glob([
    #    "include/boost/serialization/**/*.hpp",
    #    "include/boost/archive/**/*.hpp",
    #]),
    #strip_include_prefix = "include",
)
""",
    path = "/usr",
)

new_local_repository(
    name = "cslim",
    build_file_content = """
package(default_visibility = ["//visibility:public"])
cc_library(
    name = "cslim",
    srcs = ["lib/libCSlim.a"],
    hdrs = [
        "include/ExecutorC/SymbolTable.h",
        "include/Com/SocketServer.h",
        "include/Com/TcpComLink.h",
        "include/CSlim/SlimList.h",
        "include/CSlim/Slim.h",
        "include/CSlim/SlimListSerializer.h",
        "include/CSlim/SlimListDeserializer.h",
        "include/CSlim/Fixtures.h",
        "include/CSlim/SlimUtil.h",
        "include/CSlim/SlimConnectionHandler.h",
        "include/CSlim/StatementExecutor.h",
        "include/CSlim/ListExecutor.h",
    ],
    #includes = [
    #    "include/Com",
    #    "include/CSlim",
    #    "include/ExecutorC",
    #],
    strip_include_prefix = "include",
)
""",
    path = "/home/racko/tmp/cslim",
)
