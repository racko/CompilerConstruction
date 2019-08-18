add_library(ranges INTERFACE)

target_include_directories(ranges
  INTERFACE
    .
)

add_library(poker
  determine_hand_rank.cpp
  hand_rank.cpp
  poker.cpp
  poker_dfa.cpp
  poker_dfa_serialization.cpp
  rank_count.cpp
  rank_set.cpp
  ranks.cpp
  significant_ranks.cpp
  sorted_cards.cpp
  sorted_cards_evaluation.cpp
  straight.cpp
  terminals.cpp
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
        poker
        catch2::catch2
        test_main
    )

    add_test(poker_test poker_test.exe)
endif()
