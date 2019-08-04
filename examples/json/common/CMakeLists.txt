add_library(json-common
    src/jsonValue.cpp
    src/json_token.cpp
)

target_include_directories(json-common
  PUBLIC
    include
)

target_link_libraries(json-common
  PUBLIC
    util
)
