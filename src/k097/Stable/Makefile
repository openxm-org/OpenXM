

all : mk mkdata sm1symbol.h

sm1symbol.h : mk.c mkdata stable
	bash stable 
	touch sm1symbol.h
	/bin/rm sm1symbol.h
	./mkdata <stable.out >sm1symbol.h

mk :  mk.c
	gcc -o mk mk.c

mkdata :  mkdata.c
	gcc -o mkdata mkdata.c

clean :
	rm -f *.o mk mk2 mkdata stable.out stable.tmp
