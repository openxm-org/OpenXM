# $OpenXM$
OpenXM_HOME=../../../..
OpenXM_include=${OpenXM_HOME}/include
OpenXM_lib=${OpenXM_HOME}/lib
CFLAGS = -g -D_BSD_SOURCE
YACC=yacc -d

all: polymake2tfb

## Data converters
kanlibConverter=yylex_polymake.o yy_polymake.tab.o

polymake2tfb: yymain_polymake.o yylex_polymake.o yy_polymake.tab.o
	${CC} $(CFLAGS) -o polymake2tfb yymain_polymake.o yylex_polymake.o yy_polymake.tab.o -L${OpenXM_lib} -lgc
yymain_polymake.o:yymain_polymake.c yy_polymake.tab.h
yylex_polymake.o:yylex_polymake.c yylex_polymake.h yy_polymake.tab.h
yy_polymake.tab.o : yy_polymake.tab.c 
yy_polymake.tab.c : yy_polymake.y yylex_polymake.h
	${YACC} -b yy_polymake -p PM yy_polymake.y
yy_polymake.tab.h : yy_polymake.y yylex_polymake.h
	${YACC} -b yy_polymake -p PM yy_polymake.y

clean:
	rm -r -f *.o *~ *.tab.* polymake2tfb
