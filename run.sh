#!/bin/bash
rm bin
gcc -Wall -I/usr/include/ -c -o main.o main.cpp
gcc -Wall -I/usr/include/ -o bin -L/usr/X11R6/lib main.o -lX11 -lXi -lXmu -lglut -lGL -lGLU -lm
rm main.o
./bin
