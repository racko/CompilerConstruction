add_executable(WhileMain.exe
    WhileMain.cpp
    While.cpp
)

target_link_libraries(WhileMain.exe
  PRIVATE
    util
)

if(BUILD_TESTING)
endif()
