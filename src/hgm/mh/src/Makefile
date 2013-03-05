## $OpenXM: OpenXM/src/hgm/mh/src/Makefile,v 1.9 2013/03/05 00:43:29 takayama Exp $
OPT=-g -DSTANDALONE
OPT2=-g
OPT3=-g -DSTANDALONE2
# gdb hgm_w-n with the -g option.
all: hgm_w-n hgm_jack-n mh

hgm_w-n: code-n.o rk.o wmain.c sfile.o sfile.h
	${CC} -o hgm_w-n ${OPT3} wmain.c rk.o code-n.o sfile.o

rk.o: rk.c
	${CC} -c ${OPT} rk.c
code-n.o: code-n.c
	${CC} -c ${OPT} code-n.c
sfile.o: sfile.c sfile.h
	${CC} -c ${OPT} sfile.c

test1: test1.c wmain.c sfile.h code-n.o rk.o sfile.o
	${CC} ${OPT2} -o test1 test1.c wmain.c rk.o code-n.o sfile.o

hgm_jack-n: jack-n.c sfile.o sfile.h
	${CC} ${OPT3} -o hgm_jack-n jack-n.c sfile.o -lm

test2: test2.c jack-n.c sfile.h sfile.o
	${CC} ${OPT2} -o test2 test2.c jack-n.c sfile.o -lm

wmain.o : wmain.c sfile.h
	${CC} ${OPT} -c wmain.c
jack-n.o : jack-n.c  sfile.h
	${CC} ${OPT} -c jack-n.c
mh: mh.c mh.h sfile.h jack-n.o wmain.o rk.o code-n.o sfile.o
	${CC} ${OPT} -o mh mh.c jack-n.o wmain.o rk.o code-n.o sfile.o -lm

clean:
	rm -rf *.o *~ hgm_w-n test1 hgm_jack-n

test3b:
	(cd Testdata ; ./test3b.sh )
test2.sh:
	(cd Testdata ; ./test2.sh )
test3.sh:
	(cd Testdata ; ./test3.sh )
