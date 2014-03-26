#CC=gcc

hgm_ko_nc_fb: hgm_ko_nc_fb.o
	${CC} -o hgm_ko_nc_fb hgm_ko_nc_fb.o -lgsl -lblas -lm
hgm_ko_nc_fb.o: hgm_ko_nc_fb.c
	${CC} -c -DSTANDALONE hgm_ko_nc_fb.c
#todo, the install folder should be changeable by configure.
install: hgm_ko_nc_fb
	install hgm_ko_nc_fb ../../../../bin
clean:
	rm -f *.o *~ hgm_ko_nc_fb

