#include <GL/gl.h>
#include "tree.h" // for treenode
#include "coord.h" // for bounding boxes

#ifndef _REIGON_H_
#define _REIGON_H_

typedef struct reigon
{
   point viewport;
   point viewsize;
   point c_origin;
   point c_dim;
   treenode *hbar;
   treenode *vbar;
} reigon;

int destroy_reigon(treenode *dis);
treenode *create_reigon(treenode *parent, GLfloat x, GLfloat y, GLfloat z, GLfloat width, GLfloat height, GLfloat rotx, GLfloat roty, GLfloat rotz, int visible);
void draw_reigon(treenode *this);
int resize_reigon(treenode *this, point *size);
int move_reigon(treenode *this, point *size);
int calc_control_bounds(treenode *thing);
int union_reigon_size(treenode *thing);

#endif
