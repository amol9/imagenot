
MKDIR = mkdir -p
CC=gcc


make_dirs:
	${MKDIR} bin
	${MKDIR} bin/c
	${MKDIR} bin/java


build: build_c, build_java

build_c: make_dirs imagebot

imagebot:
	${CC} -Wall -g imagenot.c -o bin/c/imagenot `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`


build_java: make_dirs
	javac -classpath /usr/share/java/gtk.jar -d bin/java First.java




#clean:



run_c: build_c
	./bin/c/imagenot


run_java: build_java
	java -classpath /usr/share/java/gtk.jar:bin/java First


run_py: 
	python first.py



