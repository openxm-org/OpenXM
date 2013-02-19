## $OpenXM$
OPT=-g -DSTANDALONE
# gdb mh-w-n with the -g option.
all: mh-w-n

mh-w-n: code-n.o rk.o wmain.o
	${CC} -o mh-w-n ${OPT} wmain.o rk.o code-n.o

wmain.o: wmain.c
	${CC} -c ${OPT} wmain.c
rk.o: rk.c
	${CC} -c ${OPT} rk.c
code-n.o: code-n.c
	${CC} -c ${OPT} code-n.c

clean:
	rm -rf *.o *~ mh-w-n