## $OpenXM: OpenXM/src/hgm/mh/src/Makefile,v 1.21 2016/10/30 01:10:18 takayama Exp $
OPT=-g -DSTANDALONE -Wstrict-prototypes 
OPT2=-g -Wstrict-prototypes 
OPT3=-g -DSTANDALONE2 -Wstrict-prototypes 
OPT4=-g -DSTANDALONE -Wpedantic -Wstrict-prototypes 
#
# For using AddressSanitizer, apt-get lib32asan0-dbg (for amd64). ulimit -v
#
#OPT=-g -DSTANDALONE -fsanitize=address -fno-omit-frame-pointer 
#OPT2=-g -fsanitize=address -fno-omit-frame-pointer 
#OPT3=-g -DSTANDALONE2 -fsanitize=address -fno-omit-frame-pointer
# 
HEADERS=config.h odeiv_util.h t-gsl_errno.h \
        t-gsl_math.h t-gsl_odeiv.h t-gsl_sys.h \
        t-gsl_types.h
LINKOPT=../../gsl-t-1/src/gsl-t-1.a -lm
all: make-link hgm_w-n hgm_jack-n mh

hgm_w-n: code-n.o code-n-2f1.o rk.o wmain.c usage-w-n.h sfile.o sfile.h
	${CC} -o hgm_w-n ${OPT3} wmain.c rk.o code-n.o code-n-2f1.o sfile.o ${LINKOPT}

rk.o: rk.c mh.h ${HEADERS}
	${CC} -c ${OPT} rk.c
code-n.o: code-n.c mh.h
	${CC} -c ${OPT} code-n.c
sfile.o: sfile.c sfile.h
	${CC} -c ${OPT} sfile.c

test1: test1.c wmain.c sfile.h code-n.o rk.o sfile.o
	${CC} ${OPT2} -o test1 test1.c wmain.c rk.o code-n.o sfile.o

hgm_jack-n: jack-n.c sfile.o sfile.h usage-jack-n.h
	${CC} ${OPT3} -o hgm_jack-n jack-n.c sfile.o -lm

test2: test2.c jack-n.c sfile.h sfile.o
	${CC} ${OPT2} -o test2 test2.c jack-n.c sfile.o -lm

wmain.o : wmain.c sfile.h mh.h
	${CC} ${OPT} -c wmain.c
jack-n.o : jack-n.c  sfile.h
	${CC} ${OPT} -c jack-n.c
mh: mh.c mh.h sfile.h jack-n.o wmain.o rk.o code-n.o code-n-2f1.o sfile.o
	${CC} ${OPT} -o mh mh.c jack-n.o wmain.o rk.o code-n.o code-n-2f1.o sfile.o ${LINKOPT}

test3: sfile.c sfile.h
	${CC} -g -DTEST -DSTANDALONE sfile.c -o test3

genusage: genusage.c
	${CC} -o genusage genusage.c
usage-jack-n.h: usage-jack-n.txt genusage
	./genusage <usage-jack-n.txt >usage-jack-n.h
usage-w-n.h: usage-w-n.txt genusage
	./genusage <usage-w-n.txt >usage-w-n.h

code-n-2f1.o: code-n-2f1.c mh.h
	${CC} -c ${OPT} code-n-2f1.c

make-link: make-link-p
	-if [ ! -f .link_done ] ; then \
	for i in ${HEADERS}; do \
		ln -s ../../gsl-t-1/src/$$i . ; \
	done ; \
	touch .link_done ; \
	fi
make-link-p:
	-if [ ! -f .link_done_p ] ; then \
	ln -s ../../oxprint/oxprint.h . ; \
	touch .link_done_p ; \
	fi
clean:
	rm -rf *.o *~ hgm_w-n test1 hgm_jack-n .link_done*  test2 test3

test3b:
	(cd Testdata ; ./test3b.sh )
test2.sh:
	(cd Testdata ; ./test2.sh )
test3.sh:
	(cd Testdata ; ./test3.sh )
test2-a.sh:
	(cd Testdata ; ./test2-a.sh )
	@echo "Test of the adaptive method."

install: all
	cp hgm_w-n hgm_jack-n ../../../../bin
distclean: clean
	rm -f ../../../../bin/hgm_w-n ../../../../bin/hgm_jack-n

