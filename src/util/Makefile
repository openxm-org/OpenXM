# $OpenXM$
OpenXM_ROOT=../../../OpenXM
OpenXM_bin=$(OpenXM_ROOT)/bin
OpenXM_lib=$(OpenXM_ROOT)/lib
CC=gcc

all : oxweave

oxweave : oxweave.c
	gcc    -D_BSD_SOURCE -o oxweave -g oxweave.c


install: all
	cp oxweave ${OpenXM_bin}
