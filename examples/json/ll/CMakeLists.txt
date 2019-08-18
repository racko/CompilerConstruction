add_library(json-ll
    src/jsonLLGrammar.cpp
    src/jsonLLParser.cpp
)

target_include_directories(json-ll
  PUBLIC
    include
)

target_link_libraries(json-ll
  PUBLIC
    json-common
    util
)
