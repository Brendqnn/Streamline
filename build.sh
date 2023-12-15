#!/bin/bash

echo "Compiling..."

CFLAGS="-Wall -g $(pkg-config --cflags gtk+-3.0)"
LDFLAGS="$(pkg-config --libs gtk+-3.0) -lavformat -lavcodec -lavutil"

SOURCE_FILES="src/*.c"
OUTPUT_EXE="main"

cc $CFLAGS -o $OUTPUT_EXE $SOURCE_FILES $LDFLAGS

echo "Build Successful."

clean() {
    rm -f $OUTPUT_EXE
}
