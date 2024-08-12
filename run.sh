#!/bin/bash
rm bin
# Compile, but do not link.
# Use something like `pkg-config --cflags freetype2` when using a new library in the project.
# Note that this compile command is also present in `regenerate_compile_commands.sh`.
gcc -g -Wall -I/usr/include/ -I/usr/include/freetype2 -I/usr/include/libpng16 -DWITH_GZFILEOP -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/sysprof-6 -pthread -c -o main.o main.cpp
# Link.
# Use something like `pkg-config --libs freetype2` when using a new library in the project.
gcc -g -Wall -I/usr/include/ -o bin -L/usr/X11R6/lib main.o -lX11 -lXi -lXmu -lglut -lGL -lGLU -lm -lGLEW -lfreetype
rm main.o
./bin
