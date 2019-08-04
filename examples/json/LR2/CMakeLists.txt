add_library(json-lr2
    src/jsonLRGrammar2.cpp
    src/jsonLRParser2.cpp
)

target_include_directories(json-lr2
  PUBLIC
    include
)

target_link_libraries(json-lr2
  PUBLIC
    json-common
    util
)
