# libraries

# netUtils library
add_library(${TARGET_NET_UTILS} STATIC)
target_sources(${TARGET_NET_UTILS}
        PRIVATE netUtils.c
        PUBLIC  utils.c
        )
# using PUBLIC propagates these directories to server and keys targets
# which need it to include utils.h & netUtils.h
target_include_directories(${TARGET_NET_UTILS} PUBLIC ../include)

# keys dynamic library
add_library(${TARGET_KEYS} SHARED)
target_sources(${TARGET_KEYS} PRIVATE keys.c)
target_link_libraries(${TARGET_KEYS} PRIVATE ${TARGET_NET_UTILS})
# using PUBLIC propagates this directory to client target, which needs it to include utils.h & keys.h
target_include_directories(${TARGET_KEYS} PUBLIC ../include)

# dbms library code
add_subdirectory(dbms)
