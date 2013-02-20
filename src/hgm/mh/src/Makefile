## $OpenXM: OpenXM/src/hgm/mh/src/Makefile,v 1.2 2013/02/19 08:03:14 takayama Exp $
OPT=-g -DSTANDALONE
OPT2=-g
# gdb mh-w-n with the -g option.
all: mh-w-n

mh-w-n: code-n.o rk.o wmain.c sfile.o sfile.h
	${CC} -o mh-w-n ${OPT} wmain.c rk.o code-n.o sfile.o

rk.o: rk.c
	${CC} -c ${OPT} rk.c
code-n.o: code-n.c
	${CC} -c ${OPT} code-n.c
sfile.o: sfile.c sfile.h
	${CC} -c ${OPT} sfile.c

test1: test1.c wmain.c sfile.h code-n.o rk.o sfile.o
	${CC} ${OPT2} -o test1 test1.c wmain.c rk.o code-n.o sfile.o

clean:
	rm -rf *.o *~ mh-w-n test1