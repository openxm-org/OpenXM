#
# $OpenXM: OpenXM/src/OpenMath/Makefile,v 1.36 2000/11/27 08:15:07 ohara Exp $

OpenXM_HOME = ../..
INSTALLDIR = ${OpenXM_HOME}/lib/java-support

#JAVADIR = /usr/local/jdk1.1.8/
#JAVABINDIR = ${JAVADIR}bin/
#JAVAINCLUDEDIR = ${JAVADIR}include
#JAVAINCLUDEDIR = /usr/local/include/kaffe

JAVAC = ${JAVABINDIR}javac
JAVAH = ${JAVABINDIR}javah
JAR = ${JAVABINDIR}jar

JFLAGS = -O -g -deprecation

# TARGETS = OMproxy.class OM2OXM.class ORG/openxm/tam/OpenXM.class
TARGETS = OMproxy.class

.SUFFIXES: .java .class

.java.class:
	${JAVAC} ${JFLAGS} -d . $<

all: ${TARGETS}

configure:

clean:
	-find . -name '*~' | xargs rm -f
	-find . -name '*.class' | xargs rm -f OpenXM.jar

distclean:	clean

install: all
	-mkdir -p ${INSTALLDIR}
	find . -name '*.class' | xargs tar cf - ${TARGETS} | \
	(cd ${INSTALLDIR} ; tar xf -)

OpenXM.jar:	all
	find ORG -name '*.class' | xargs ${JAR} cf OpenXM.jar
