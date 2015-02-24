CFLAGS=-Wimplicit -Werror -Wall -g -O3 -falign-loops=2 -falign-jumps=2 -falign-functions=2 -I../../include -DFX -DXMESA

LIBS=-L/usr/X11R6/lib/ -L/usr/local/glide/lib -lX11 -lXext -lXmu -lXt -lXi -lSM -lICE -lttf -lglut -lGL -lGLU -lm -ldl

OBJS= \
     api.o \
     button.o \
     bar.o \
     barmanager.o \
     conn.o \
     coord.o \
     container.o \
     deepwindow.o \
     display.o \
     draw.o \
     event.o \
     font.o \
     init.o \
     input.o \
     glyph.o \
     group.o \
     list.o \
     menu.o \
     nodetypes.o \
     plugin.o \
     raytri.o \
     reigon.o \
     resource.o \
     text.o \
     tree.o \
     tritri.o \
     window.o \
     wmanager.o \
     linux/linux.a

all:
	make -C linux
	make -C plugins
	make synapse

synapse: $(OBJS)
	gcc $(LIBS) -rdynamic -o synapse $(OBJS) $(LIBS)

.c.o:
	gcc $(CFLAGS) $(OCFLAGS) -c $<

clean:
	make -C linux clean
	make -C plugins clean
	rm -f *.o
	rm -f synapse
