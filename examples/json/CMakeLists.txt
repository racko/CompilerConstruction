add_library(json
    json.cpp
    jsonLexer.cpp
)

target_link_libraries(json
  PUBLIC
    json-common
    json-lr
    util
)

add_executable(json.exe
    jsonMain.cpp
)

target_link_libraries(json.exe
    json
    json-ll
    Boost::filesystem
    jsoncpp
)

if(BUILD_TESTING)
    add_executable(json_test.exe
        json_test.cpp
    )

    target_link_libraries(json_test.exe
      PRIVATE
        catch2
        json
        json-ll
        json-lr
        json-lr2
        test_main
    )

    add_test(json_test json_test.exe)
endif()

add_subdirectory(common)
add_subdirectory(LL)
add_subdirectory(LR)
add_subdirectory(LR2)
