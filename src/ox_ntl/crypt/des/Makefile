# $OpenXM$

PROG=des

CC ?= gcc

CFLAGS+=-Wall -g

SRCS=block.c des.c destest.c

OBJC=$(SRCS:.c=.o)

all:: $(PROG)

des: $(OBJC)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJC) -o $@


.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	-rm -f $(OBJC) $(PROG)


