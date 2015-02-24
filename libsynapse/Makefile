CFLAGS=-Wimplicit -Werror -Wall -g -O3

LIBS=

OBJS= \
  main.o \
  libsynapse.o \
  ls_error.o \
  ls_ctrlapi.o \
  ls_wmapi.o \
  ls_idmap.o \
  ls_event.o \
  socketcomm.o

libsynapse.so: $(OBJS)
	gcc -o libsynapse.so -shared $(CFLAGS) $(OBJS)

.c.o:
	gcc $(CFLAGS) -c $<

.C.o:
	c++ $(CFLAGS) -c $<

clean:
	rm -f *.o
	rm -f libsynapse.so
