# New Ports Collection Makefile for:	OpenXM
#
# $OpenXM: OpenXM/misc/packages/FreeBSD/openxm-ports/openxm/Makefile,v 1.3 2000/10/29 17:50:39 maekawa Exp $
#

PORTNAME=	openxm
PORTVERSION=	1.1.3
CATEGORIES=	math
MASTER_SITES=	ftp://ftp.math.sci.kobe-u.ac.jp/pub/OpenXM/${PORTVERSION}/
DISTNAME=	openxm-${PORTVERSION}

MAINTAINER=	maekawa@math.sci.kobe-u.ac.jp

BUILD_DEPENDS=	${PREFIX}/jdk1.1.8/bin/javac:${PORTSDIR}/java/jdk \
		jlatex2html:${PORTSDIR}/japanese/latex2html \
		jmakeinfo:${PORTSDIR}/japanese/texinfo \
		texi2html:${PORTSDIR}/textproc/texi2html \

MAN1=		sm1.1 openxm.1

.include <bsd.port.mk>
