#
# $OpenXM: OpenXM/src/OpenMath/Makefile,v 1.12 2000/01/17 16:53:43 tam Exp $


JAVADIR = /usr/local/jdk1.1.8/
#JAVABINDIR = $(JAVADIR)bin/
LIBDIR = ../../lib/java-support

TARGETS = OpenXM.jar OM2OXM.class OMproxy.class libnative.so


all:	configure $(TARGETS)


configure:


clean:
	find . -name '*.class' \
		-exec echo rm -f {} \; \
		-exec rm -f {} \;
	-rm -f *.so *.h *.jar


distclean:	clean
	find . -name '*~' \
		-exec echo rm -f {} \; \
		-exec rm -f {} \;


install:	all
	if ! test -d $(LIBDIR); then \
		mkdir -p $(LIBDIR); \
	fi
	cp `find . -name '*.class'` $(TARGETS) $(LIBDIR)


install2:


test:
	$(JAVABINDIR)javac -d . -deprecation -g *.java
	make libnative.so


.SUFFIXES:	.java .class


.java.class:
	$(JAVABINDIR)javac -d . -O $<


OpenXM.jar:	JP/ac/kobe_u/math/tam/OpenXM/*.java
	$(JAVABINDIR)javac -d . -O $?
	touch $@
	-@#$(JAVABINDIR)jar cf OpenXM.jar `find JP/ -name '*.class'`


OMproxy.h:	OMproxy.class
	-$(JAVABINDIR)javah -jni $*


libnative.so:	native.c OMproxy.h
	cc -o libnative.so native.c \
		-shared `find $(JAVADIR)/include/ -type d | sed 's/^/-I/'`
