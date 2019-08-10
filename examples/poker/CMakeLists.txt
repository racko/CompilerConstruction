add_library(poker
    poker.cpp
)

target_link_libraries(poker
  PUBLIC
    util
  PRIVATE
    Boost::serialization
    Boost::filesystem
)

if(BUILD_TESTING)
    add_executable(poker_test.exe
        poker_test.cpp
    )

    target_link_libraries(poker_test.exe
      PRIVATE
        catch2
        poker
        test_main
    )

    add_test(poker_test poker_test.exe)
endif()
