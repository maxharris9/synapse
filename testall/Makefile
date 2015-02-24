CFLAGS=-Wimplicit -Werror -Wall -g -O3 -I../../include -I../include

LIBS=-L../libsynapse -lsynapse

testall: testall.c
	gcc $(CFLAGS) $(LIBS) -o testall testall.c

clean:
	rm -f testall
