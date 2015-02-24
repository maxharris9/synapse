#ifndef _BAR_H_
#define _BAR_H_

#include "synapse.h"
#include "tritri.h"
#include "container.h"
#include "event.h"
#include "tree.h"

typedef struct bar
{
   int min, max, window, value;
} bar;

int get_progbar(treenode *thing);
int get_scrollbar_value(treenode *thing);
int change_slider_value(treenode *thing, int value);
int get_slider_value(treenode *thing);

treenode *create_bar(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height,
GLfloat rotx, GLfloat roty, GLfloat rotz, int min, int max, int value, int window, GLfloat *color, int type, int subtype, int visible);
void draw_bar(treenode *dis);
void color_bar(treenode *dis, GLfloat *c);
void destroy_bar(treenode *dis);
void move_slider(treenode *dis, point *pt);
int change_progress_bar(treenode *that, GLfloat percentage);
int event_bar(struct treenode *target, struct treenode *node, int event, int modifier, point *where, unsigned long usec);
void draw_slider_tray(treenode *dis);
void draw_tray(treenode *dis);
int resize_bar(treenode *thing, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz);

#endif
