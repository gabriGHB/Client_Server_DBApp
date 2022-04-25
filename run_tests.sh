#!/bin/sh

# set up environment variables for server IP & port
export IP_TUPLES=$1
export PORT_TUPLES=$2

cd build
app/server $PORT_TUPLES > /dev/null & 2>&1
ctest -VV
pkill -SIGINT '^server$'