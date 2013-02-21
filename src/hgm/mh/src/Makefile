## $OpenXM: OpenXM/src/hgm/mh/src/Makefile,v 1.5 2013/02/21 01:31:03 takayama Exp $
OPT=-g -DSTANDALONE
OPT2=-g
# gdb hgm_w-n with the -g option.
all: hgm_w-n hgm_jack-n 

hgm_w-n: code-n.o rk.o wmain.c sfile.o sfile.h
	${CC} -o hgm_w-n ${OPT} wmain.c rk.o code-n.o sfile.o

rk.o: rk.c
	${CC} -c ${OPT} rk.c
code-n.o: code-n.c
	${CC} -c ${OPT} code-n.c
sfile.o: sfile.c sfile.h
	${CC} -c ${OPT} sfile.c

test1: test1.c wmain.c sfile.h code-n.o rk.o sfile.o
	${CC} ${OPT2} -o test1 test1.c wmain.c rk.o code-n.o sfile.o

hgm_jack-n: jack-n.c sfile.o sfile.h
	${CC} ${OPT} -o hgm_jack-n jack-n.c sfile.o -lm

test2: test2.c jack-n.c sfile.h sfile.o
	${CC} ${OPT2} -o test2 test2.c jack-n.c sfile.o -lm


clean:
	rm -rf *.o *~ hgm_w-n test1 hgm_jack-n

test3b:
	(cd Testdata ; ./test3b.sh )
