CC=gcc -g
INCLUDE= -I/sw/include -I/usr/X11R6/include 
LIB=-L/sw/lib/mesa -L/usr/X11R6/lib -lGL -lGLU -lglut -lm
## /sw を優先にする. 混じるとうまく表示されない.  [mac-mini]

usage:
	@echo "make glray3  glicosa"
glray3: glray3.c
	${CC} -DMYTEST glray3.c ${INCLUDE} ${LIB} -o glray3

glicosa: glicosa.c
	${CC} -DMYTEST glicosa.c ${INCLUDE} ${LIB} -o glicosa

clean:
	rm -f glicosa glray3 *.o *~

