cmake_minimum_required(VERSION 3.16)
project(ninja_solver CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -ffast-math)
    endif()
endif()

# Game library (existing poker code)
add_library(poker_game
    game/Card.cpp
    game/Deck.cpp
    game/HandEvaluation.cpp
)
target_include_directories(poker_game PUBLIC ${CMAKE_SOURCE_DIR})

# Solver core library
add_library(solver_core
    solver/core/CardAdapter.cpp
    solver/core/Board.cpp
    solver/core/HandRanking.cpp
    solver/core/Range.cpp
)
target_include_directories(solver_core PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(solver_core PUBLIC poker_game)

# Tests
enable_testing()

# Test executable for solver core
add_executable(test_solver_core
    tests/solver/test_core.cpp
)
target_link_libraries(test_solver_core solver_core)
add_test(NAME test_solver_core COMMAND test_solver_core)

# Convenience target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_solver_core
)
