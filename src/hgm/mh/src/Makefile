## $OpenXM: OpenXM/src/hgm/mh/src/Makefile,v 1.6 2013/02/21 07:30:56 takayama Exp $
OPT=-g -DSTANDALONE
OPT2=-g
# gdb hgm_w-n with the -g option.
all: hgm_w-n hgm_jack-n mh

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

wmain.o : wmain.c sfile.h
	${CC} ${OPT2} -c wmain.c
jack-n.o : jack-n.c  sfile.h
	${CC} ${OPT2} -c jack-n.c
mh: mh.c mh.h sfile.h jack-n.o wmain.o rk.o code-n.o sfile.o
	${CC} ${OPT} -o mh mh.c jack-n.o wmain.o rk.o code-n.o sfile.o

clean:
	rm -rf *.o *~ hgm_w-n test1 hgm_jack-n

test3b:
	(cd Testdata ; ./test3b.sh )
