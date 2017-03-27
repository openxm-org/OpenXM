#CC=gcc
AR=ar
RANLIB=ranlib
OBJS=t-control.o t-cstd.o t-evolve.o t-rkf45.o t-step.o t-coerce.o t-error.o 
.c.o: 
	${CC} -c -O2 $<
gsl-t-1.a: make-link-p ${OBJS}
	if [ ${AR} = ar ]; then \
	ar ru gsl-t-1.a *.o ; \
	else \
	libtool -static -o gsl-t-1.a *.o ; \
	fi
	${RANLIB} gsl-t-1.a

make-link-p:
	-if [ ! -f .link_done_p ] ; then \
	ln -s ../../oxprint/oxprint.h . ; \
	touch .link_done_p ; \
	fi
clean:
	rm -f *.o *~ gsl-t-1.a oxprint.h .link_done_p

all: gsl-t-1.a
distclean: clean
install: all
# Only the MacOS X accepts the above libtool options.
## test implementation including the minimum codes from the gsl 4.47
## @s/2012/12/23-gsl* (gsl 4.47 code).  See ode-initval, err, sys
## h-mle/SO/Prog/hgm

#gsl_errno.h, gsl_math.h, gsl_matrix.h, gsl_odeiv.h, gsl_sys.h
