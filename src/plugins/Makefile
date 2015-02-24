CFLAGS=-Wimplicit -Werror -Wall -O3 -I..

OBJS= \
     test.o

test.so: $(OBJS)
	gcc -shared -nostdlib -o test.so $(OBJS)

.c.o:
	gcc $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f test.so
