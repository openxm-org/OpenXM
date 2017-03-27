# $OpenXM$
usage:
	@echo "targets are all, install, distclean. Only for mh for now."

all:
	(cd gsl-t-1/src ; make all)
	(cd mh/src ; make all)

install: all
	(cd mh/src ; make install)

clean:
	(cd gsl-t-1/src ; make clean)
	(cd mh/src ; make clean)

distclean: clean
	(cd mh/src ; make distclean)


