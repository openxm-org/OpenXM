#CC=gcc
HEADERS=config.h odeiv_util.h t-gsl_errno.h \
        t-gsl_math.h t-gsl_odeiv.h t-gsl_sys.h \
        t-gsl_types.h
Opt= -Wstrict-prototypes 
hgm_so3_nc: make-link so3_nc.o 
	${CC} ${Opt} -o hgm_so3_nc so3_nc.o ../../gsl-t-1/src/gsl-t-1.a -lm
so3_nc.o : so3_nc.c
	${CC} ${Opt} -c -DSTANDALONE so3_nc.c

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
	rm -f *.o *~ hgm_so3_nc  .link_done* 
	for i in ${HEADERS}; do \
		rm -f $$i ; \
	done 

