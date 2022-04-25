# tests with GoogleTest

# tests for keys library (client API, a running server is required)

set(TARGET_KEYS_TESTS keys_tests)
add_executable(${TARGET_KEYS_TESTS})
target_sources(${TARGET_KEYS_TESTS} PRIVATE keys_tests.cpp)
target_link_libraries(${TARGET_KEYS_TESTS}
        PRIVATE gtest_main
                ${TARGET_KEYS}
        )
add_test(
        NAME ${TARGET_KEYS_TESTS}
        COMMAND ${TARGET_KEYS_TESTS}
)
