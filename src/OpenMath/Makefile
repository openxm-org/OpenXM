#
# $OpenXM: OpenXM/src/OpenMath/Makefile,v 1.6 1999/12/09 00:32:36 tam Exp $


JAVADIR = /usr/local/jdk1.1.8
LIBDIR = ../../lib/java-support


all:	install


clean:
	find . -name '*.class' \
		-exec echo rm -f {} \; \
		-exec rm -f {} \;
	-rm -f *.so *.h *.jar


distclean:	clean
	find . -name '*~' \
		-exec echo rm -f {} \; \
		-exec rm -f {} \;


test:
	$(JAVADIR)/bin/javac -d . -deprecation -g *.java
	make libnative.so


install:	OpenXM.jar OM2OXM.class OMproxy.class libnative.so
	if ! test -d $(LIBDIR); then \
		mkdir -p $(LIBDIR); \
	fi
	cp ${.ALLSRC} $(LIBDIR)


.SUFFIXES:	.java .class


.java.class :
	$(JAVADIR)/bin/javac -d . -O ${.IMPSRC}


OpenXM.jar:	JP/ac/kobe_u/math/tam/OpenXM/{OpenXM,*}.class
	@$(JAVADIR)/bin/javac -d . -O JP/ac/kobe_u/math/tam/OpenXM/OpenXM.java
	@#$(JAVADIR)/bin/javac -d . -O ${.OODATE}
	$(JAVADIR)/bin/jar cf OpenXM.jar `find JP/ -name '*.class'`


OMproxy.h:	OMproxy.class
	$(JAVADIR)/bin/javah -jni ${.PREFIX}


libnative.so:	native.c OMproxy.h
	cc -o libnative.so native.c \
		-shared `find $(JAVADIR)/include/ -type d | sed 's/^/-I/'`
