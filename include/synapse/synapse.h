#include <GL/gl.h>

#ifndef _SYNAPSE_H_
#define _SYNAPSE_H_

#define TRUE 1
#define FALSE 0

typedef struct {
  GLfloat x, y, z;
} Point;

typedef void (*init_callback) (int argc, char **argv);
typedef void (*event_callback) (int id, int type, int modifier, Point *where, int usec);
typedef void (*idle_callback) (void);

int syn_main(int argc, char **argv);
void syn_quit(void);
int syn_set_callback_init(init_callback callback);
int syn_set_callback_event(event_callback callback);
int syn_set_callback_idle(idle_callback callback);

#endif
