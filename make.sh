#! /bin/bash

gcc -Wall -g first.c -o first.o `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`
