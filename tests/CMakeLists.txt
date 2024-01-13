include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(demo
        demo.cpp
)
target_link_libraries(demo PRIVATE enquirer)

add_executable(unit-tests
        tests.cpp
)
target_link_libraries(unit-tests PRIVATE enquirer gtest_main gtest gmock)
gtest_discover_tests(unit-tests)
