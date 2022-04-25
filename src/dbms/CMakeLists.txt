# dbms library

add_library(${TARGET_DBMS} STATIC)
target_sources(${TARGET_DBMS}
        PRIVATE     dbms.c
                    dbmsUtils.c
        PUBLIC      ../utils.c
        )
# using PUBLIC propagates this directory to server target
# which needs it to include dbms.h
target_include_directories(${TARGET_DBMS} PUBLIC ../../include)
