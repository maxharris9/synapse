#ifndef _DEEPWINDOW_H_
#define _DEEPWINDOW_H_

#include "synapse.h"
#include "event.h"
#include "tree.h"

treenode *create_deepwindow(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
         GLfloat depth, GLfloat rotx, GLfloat roty, GLfloat rotz, GLfloat *color, int type, int subtype, char *title);

void draw_deepwindow(treenode *dis);

void destroy_deepwindow(treenode *dis);

int event_deepwindow(treenode *object, int event, int dsize, void *data);

#endif
