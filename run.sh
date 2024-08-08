#!/bin/bash
rm bin
gcc -g -Wall -I/usr/include/ -c -o main.o main.cpp
gcc -g -Wall -I/usr/include/ -o bin -L/usr/X11R6/lib main.o -lX11 -lXi -lXmu -lglut -lGL -lGLU -lm -lGLEW
rm main.o
./bin
