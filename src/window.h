#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "synapse.h"
#include "tritri.h"
#include "container.h"
#include "event.h"
#include "tree.h"

treenode *create_window(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
           GLfloat rotx, GLfloat roty, GLfloat rotz, GLfloat *color, int type, int subtype, char *title, int visible);
void draw_window(treenode *dis);
void draw_flatwindow(treenode *dis);
void draw_dialog(treenode *dis);
void move_window(treenode *dis, point *pt);
void twist_window(treenode *dis, point *pt, point *around);
void resize_window(treenode *dis, point *pt);
void color_window(treenode *dis, GLfloat *c);
int destroy_window(treenode *dis);
void minimize_window(treenode *dis);
void cycle_active_window(void);
int event_window(struct treenode *target, struct treenode *node, int event, int 
modifier, point *where, unsigned long usec);

#endif
