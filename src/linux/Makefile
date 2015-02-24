CFLAGS=-Wimplicit -Werror -Wall -g -O3 -I..

OBJS= \
     eachfile.o \
     plugin.o

linux.a: $(OBJS)
	ar -r linux.a $(OBJS)

.c.o:
	gcc $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f linux.a
