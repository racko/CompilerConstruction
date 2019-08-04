add_library(json-lr
    src/jsonLRGrammar.cpp
    src/jsonLRParser.cpp
)

target_include_directories(json-lr
  PUBLIC
    include
)

target_link_libraries(json-lr
  PUBLIC
    json-common
    util
)
