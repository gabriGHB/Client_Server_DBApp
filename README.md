# Client_Server_DBApp

app: source code for client and server executables

build: directory used to build the project; create it if it doesn't exist

extern: directory that includes googletest; required for unittests; create it if it doesn't exist

include: header files

    dbms: header files for DBMS library

        dbms.h: function prototypes used for DB management; server-side API

        dbmsUtils.h: function prototypes called internally in the dbms module

    keys.h: header for keys library; client-side API
    
    netUtils.h: header for netUtils library; contains function prototypes used to send and receive stuff; network API used by both server and client

    utils.h: types, constants and function prototypes used throughout the project; useful stuff

src: library source code and auxiliary source files

    dbms: source code for DBMS library

        dbms.c: source code for DB management; server-side API

        dbmsUtils.c: source code for the function prototypes defined in dbmsUtils.h

    keys.c: source code for keys library; client-side API
    
    netUtils.c: source code for netUtils library; network API

    utils.c: source code for the function prototypes defined in utils.h

test: unittests with GoogleTest
