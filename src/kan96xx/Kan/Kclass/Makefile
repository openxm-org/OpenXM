# $OpenXM: OpenXM/src/kan96xx/Kan/Kclass/Makefile,v 1.3 2003/09/02 03:24:49 takayama Exp $
OpenXM_include=${OpenXM_HOME}/include
all : sample.o rawcmo.o indeterminate.o tree.o

## Change also ../Makefile

sample.o : ${HEADERS} sample.c
	${CC} -I${OpenXM_include} ${CFLAGS}  -c  sample.c
rawcmo.o : ${HEADERS} rawcmo.c
	${CC} -I${OpenXM_include} ${CFLAGS}  -c  rawcmo.c
indeterminate.o : ${HEADERS} indeterminate.c
	${CC} -I${OpenXM_include} ${CFLAGS}  -c  indeterminate.c
tree.o : ${HEADERS} tree.c
	${CC} -I${OpenXM_include} ${CFLAGS}  -c  tree.c
