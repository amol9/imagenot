MKDIR=mkdir -p
CC=gcc
RM=rm -f
CP=cp
INSTALL_PATH=/usr/bin
SHARE_PATH=/usr/share
BIN=bin
PRINTF=printf
CHMOD_X=chmod +x
JAVA=java
JAVAC=javac
PYTHON=python


.DEFAULT_GOAL = all


make_dirs:
	${MKDIR} ${BIN}
	${MKDIR} ${BIN}/c
	${MKDIR} ${BIN}/java


build_c: make_dirs imagenot


imagenot:
	${CC} -Wall -g imagenot.c -o ${BIN}/c/imagenot `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`


build_java: make_dirs
	${JAVAC} -classpath ${SHARE_PATH}/java/gtk.jar -d ${BIN}/java ImageNot.java


all: build_c build_java


clean:
	${RM} ${BIN}/c/*
	${RM} ${BIN}/java/*


install: install_c


install_c: build_c
	${CP} ${BIN}/c/imagenot ${INSTALL_PATH}


install_java: build_java
	${MKDIR} ${SHARE_PATH}/imagenot
	${CP} ${BIN}/java/*.* ${SHARE_PATH}/imagenot
	${RM} ${INSTALL_PATH}/imagenot
	${PRINTF} "#!/bin/bash\n${JAVA} -classpath ${SHARE_PATH}/java/gtk.jar:${SHARE_PATH}/imagenot ImageNot \$$1 \$$2" > ${INSTALL_PATH}/imagenot
	${CHMOD_X} ${INSTALL_PATH}/imagenot


install_py:
	${MKDIR} ${SHARE_PATH}/imagenot
	${CP} imagenot.py ${SHARE_PATH}/imagenot
	${RM} ${INSTALL_PATH}/imagenot
	${PRINTF} "#!/bin/bash\n${PYTHON} ${SHARE_PATH}/imagenot/imagenot.py \$$1 \$$2" > ${INSTALL_PATH}/imagenot
	${CHMOD_X} ${INSTALL_PATH}/imagenot

