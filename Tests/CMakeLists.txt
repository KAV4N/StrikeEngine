cmake_minimum_required(VERSION 3.16)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Collect all test sources
file(GLOB_RECURSE TEST_SOURCES
    "unit/*.cpp"
    "unit/*.h"
)

add_executable(StrikeTests ${TEST_SOURCES})

target_link_libraries(StrikeTests PRIVATE
    gtest_main      # provides main() automatically
    gmock_main      # mocking support, included for free
    StrikeEngine
)

target_compile_options(StrikeTests PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
        -Wno-unknown-pragmas
        -Wno-unused-parameter
    >
)


strike_setup(StrikeTests "${CMAKE_CURRENT_SOURCE_DIR}/Assets")

# Auto-discover tests so they show up in CTest and IDEs
include(GoogleTest)
gtest_discover_tests(StrikeTests)