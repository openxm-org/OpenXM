all : sample.o rawcmo.o indeterminate.o

## Change also ../Makekefile

sample.o : $(HEADERS) sample.c
	$(CC) -I$(GMP_HOME2) $(CFLAGS) $(DDD) -c  sample.c
rawcmo.o : $(HEADERS) rawcmo.c
	$(CC) -I$(GMP_HOME2) $(CFLAGS) $(DDD) -c  rawcmo.c
indeterminate.o : $(HEADERS) indeterminate.c
	$(CC) -I$(GMP_HOME2) $(CFLAGS) $(DDD) -c  indeterminate.c
