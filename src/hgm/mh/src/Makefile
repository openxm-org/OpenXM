## $OpenXM: OpenXM/src/hgm/mh/src/Makefile,v 1.12 2013/03/08 04:54:01 takayama Exp $
OPT=-g -DSTANDALONE
OPT2=-g
OPT3=-g -DSTANDALONE2
# gdb hgm_w-n with the -g option.
HEADERS=config.h odeiv_util.h t-gsl_errno.h \
        t-gsl_math.h t-gsl_odeiv.h t-gsl_sys.h \
        t-gsl_types.h
LINKOPT=../../gsl-t-1/src/gsl-t-1.a -lm
all: hgm_w-n hgm_jack-n mh

hgm_w-n: code-n.o rk.o wmain.c sfile.o sfile.h
	${CC} -o hgm_w-n ${OPT3} wmain.c rk.o code-n.o sfile.o ${LINKOPT}

rk.o: rk.c mh.h ${HEADERS}
	${CC} -c ${OPT} rk.c
code-n.o: code-n.c mh.h
	${CC} -c ${OPT} code-n.c
sfile.o: sfile.c sfile.h
	${CC} -c ${OPT} sfile.c

test1: test1.c wmain.c sfile.h code-n.o rk.o sfile.o
	${CC} ${OPT2} -o test1 test1.c wmain.c rk.o code-n.o sfile.o

hgm_jack-n: jack-n.c sfile.o sfile.h
	${CC} ${OPT3} -o hgm_jack-n jack-n.c sfile.o -lm

test2: test2.c jack-n.c sfile.h sfile.o
	${CC} ${OPT2} -o test2 test2.c jack-n.c sfile.o -lm

wmain.o : wmain.c sfile.h mh.h
	${CC} ${OPT} -c wmain.c
jack-n.o : jack-n.c  sfile.h
	${CC} ${OPT} -c jack-n.c
mh: mh.c mh.h sfile.h jack-n.o wmain.o rk.o code-n.o sfile.o
	${CC} ${OPT} -o mh mh.c jack-n.o wmain.o rk.o code-n.o sfile.o ${LINKOPT}

test3: sfile.c sfile.h
	${CC} -g -DTEST -DSTANDALONE sfile.c -o test3

make-link:
	-if [ ! -f .link_done ] ; then \
	for i in ${HEADERS}; do \
		ln -s ../../gsl-t-1/src/$$i . ; \
	done ; \
	touch .link_done ; \
	fi
clean:
	rm -rf *.o *~ hgm_w-n test1 hgm_jack-n .link_done  test2 test3

test3b:
	(cd Testdata ; ./test3b.sh )
test2.sh:
	(cd Testdata ; ./test2.sh )
test3.sh:
	(cd Testdata ; ./test3.sh )
test2-a.sh:
	(cd Testdata ; ./test2-a.sh )
	@echo "Test of the adaptive method."
