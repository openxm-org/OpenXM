# $OpenXM: OpenXM/src/kan96xx/Kan/Kclass/Makefile,v 1.2 2000/01/16 07:55:45 takayama Exp $
OpenXM_include=${OpenXM_HOME}/include
all : sample.o rawcmo.o indeterminate.o

## Change also ../Makefile

sample.o : ${HEADERS} sample.c
	${CC} -I${OpenXM_include} ${CFLAGS}  -c  sample.c
rawcmo.o : ${HEADERS} rawcmo.c
	${CC} -I${OpenXM_include} ${CFLAGS}  -c  rawcmo.c
indeterminate.o : ${HEADERS} indeterminate.c
	${CC} -I${OpenXM_include} ${CFLAGS}  -c  indeterminate.c
