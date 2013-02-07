#CC=gcc
HEADERS=config.h odeiv_util.h t-gsl_errno.h \
        t-gsl_math.h t-gsl_odeiv.h t-gsl_sys.h \
        t-gsl_types.h
so3_nc: make-link so3_nc.o 
	${CC} -o so3_nc so3_nc.o ../../gsl-t-1/src/gsl-t-1.a -lm
so3_nc.o : so3_nc.c
	${CC} -c -DSTANDALONE so3_nc.c

make-link:
	-if [ ! -f .link_done ] ; then \
	for i in ${HEADERS}; do \
		ln -s ../../gsl-t-1/src/$$i . ; \
	done ; \
	touch .link_done ; \
	fi
clean:
	rm -f *.o *~ evalnc  .link_done
	for i in ${HEADERS}; do \
		rm -f $$i ; \
	done 

