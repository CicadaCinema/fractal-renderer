#!/bin/bash
rm compile_commands.json
# Note that this compile command is also present in `run.sh`.
bear -- gcc -g -Wall -I/usr/include/ -I/usr/include/freetype2 -I/usr/include/libpng16 -DWITH_GZFILEOP -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/sysprof-6 -pthread -c -o main.o main.cpp
