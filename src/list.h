#include <GL/gl.h>
#include "tree.h" // for treenode
#include "coord.h" // for bounding boxes

#ifndef _LIST_H_
#define _LIST_H_

typedef struct item
{
   struct item *next;
   struct item *prev;
   treenode *data;
} item;

typedef struct list
{
   treenode *sortbox;
   treenode *region;
   item *columns;
   item *items;
} list;

int destroy_list(treenode *dis);
treenode *create_list(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz, GLfloat *color, int columns, int visible);
void draw_list(treenode *this);
// int resize_list(treenode *this, point *size);
// int move_list(treenode *this, point *size);

#endif
